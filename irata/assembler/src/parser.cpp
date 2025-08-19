#include <cctype>
#include <charconv>
#include <irata/assembler/parser.hpp>
#include <irata/common/strings/strings.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace irata::assembler {

Parser::Program::Statement::Statement(Type type) : type_(type) {}

Parser::Program::Statement::Type Parser::Program::Statement::type() const {
  return type_;
}

bool Parser::Program::Statement::operator==(const Statement &other) const {
  return type() == other.type();
}

bool Parser::Program::Statement::operator!=(const Statement &other) const {
  return !(*this == other);
}

Parser::Program::StatementWithValue::StatementWithValue(Type type,
                                                        std::string_view value)
    : Statement(type), value_(value) {}

std::string Parser::Program::StatementWithValue::value() const {
  return value_;
}

bool Parser::Program::StatementWithValue::operator==(
    const Statement &other) const {
  return Statement::operator==(other) &&
         value_ == dynamic_cast<const StatementWithValue &>(other).value_;
}

Parser::Program::Comment::Comment(std::string_view value)
    : StatementWithValue(Type::Comment, value) {}

Parser::Program::Label::Label(std::string_view value)
    : StatementWithValue(Type::Label, value) {}

Parser::Program::Instruction::Arg::Arg(Type type,
                                       asm_::AddressingMode addressing_mode)
    : type_(type), addressing_mode_(addressing_mode) {}

Parser::Program::Instruction::Arg::Type
Parser::Program::Instruction::Arg::type() const {
  return type_;
}

asm_::AddressingMode
Parser::Program::Instruction::Arg::addressing_mode() const {
  return addressing_mode_;
}

bool Parser::Program::Instruction::Arg::operator==(const Arg &other) const {
  return type() == other.type() && addressing_mode() == other.addressing_mode();
}

bool Parser::Program::Instruction::Arg::operator!=(const Arg &other) const {
  return !(*this == other);
}

Parser::Program::Instruction::None::None()
    : Arg(Type::None, asm_::AddressingMode::None) {}

Parser::Program::Instruction::Immediate::Immediate(common::bytes::Byte value)
    : ArgWithValue<common::bytes::Byte>(
          Type::Immediate, asm_::AddressingMode::Immediate, value) {}

Parser::Program::Instruction::AbsoluteLiteral::AbsoluteLiteral(
    common::bytes::Word value)
    : ArgWithValue<common::bytes::Word>(Type::AbsoluteLiteral,
                                        asm_::AddressingMode::Absolute, value) {
}

Parser::Program::Instruction::AbsoluteLabel::AbsoluteLabel(
    std::string_view value)
    : ArgWithValue<std::string>(Type::AbsoluteLabel,
                                asm_::AddressingMode::Absolute,
                                std::string(value)) {}

Parser::Program::Instruction::Instruction(std::string_view instruction,
                                          std::unique_ptr<Arg> arg)
    : Statement(Type::Instruction), instruction_(instruction),
      arg_(std::move(arg)) {
  if (arg_ == nullptr) {
    throw std::runtime_error("Arg cannot be null");
  }
}

std::string Parser::Program::Instruction::instruction() const {
  return instruction_;
}

const Parser::Program::Instruction::Arg &
Parser::Program::Instruction::arg() const {
  return *arg_;
}

bool Parser::Program::Instruction::operator==(const Statement &other) const {
  if (!Statement::operator==(other)) {
    return false;
  }
  const auto &other_instruction = dynamic_cast<const Instruction &>(other);
  return instruction_ == other_instruction.instruction_ &&
         *arg_ == *other_instruction.arg_;
}

Parser::Program::ByteDirective::ByteDirective(common::bytes::Byte value)
    : Statement(Type::ByteDirective), value_(value) {}

common::bytes::Byte Parser::Program::ByteDirective::value() const {
  return value_;
}

bool Parser::Program::ByteDirective::operator==(const Statement &other) const {
  return Statement::operator==(other) &&
         value_ == dynamic_cast<const ByteDirective &>(other).value_;
}

Parser::Program::Program(std::vector<std::unique_ptr<Statement>> statements)
    : statements_(std::move(statements)) {
  for (const auto &statement : statements_) {
    if (statement == nullptr) {
      throw std::runtime_error("Statement cannot be null");
    }
  }
}

const std::vector<std::unique_ptr<Parser::Program::Statement>> &
Parser::Program::statements() const {
  return statements_;
}

bool Parser::Program::operator==(const Program &other) const {
  if (statements_.size() != other.statements_.size()) {
    return false;
  }
  for (size_t i = 0; i < statements_.size(); ++i) {
    if (*statements_[i] != *other.statements_[i]) {
      return false;
    }
  }
  return true;
}

bool Parser::Program::operator!=(const Program &other) const {
  return !(*this == other);
}

namespace {

uint16_t parse_numeric_literal(std::string_view value) {
  if (value.empty()) {
    throw std::invalid_argument("Empty numeric literal");
  }

  std::string_view numeric_string = value;
  int base = 10;
  if (value[0] == '$') {
    base = 16;
    numeric_string = value.substr(1);
  } else if (common::strings::starts_with(value, "0x") ||
             common::strings::starts_with(value, "0X")) {
    base = 16;
    numeric_string = value.substr(2);
  }

  uint16_t result = 0;
  const auto begin = numeric_string.data();
  const auto end = numeric_string.data() + numeric_string.size();
  auto [ptr, ec] = std::from_chars(begin, end, result, base);
  if (ec != std::errc() || ptr != end) {
    throw std::invalid_argument("Invalid numeric format: " +
                                std::string(numeric_string));
  }
  return result;
}

uint8_t parse_numeric_literal_byte(std::string_view value_str) {
  const uint16_t value = parse_numeric_literal(value_str);
  if (value > 0xFF) {
    std::ostringstream os;
    os << "numeric value " << std::hex << int(value)
       << " is out of range for byte";
    throw std::invalid_argument(os.str());
  }
  return static_cast<uint8_t>(value);
}

} // namespace

std::unique_ptr<Parser::Program::Instruction::Arg>
Parser::Program::Instruction::Arg::parse(std::string_view arg) {
  if (arg.empty()) {
    return std::make_unique<Parser::Program::Instruction::None>();
  }

  if (arg[0] == '#') {
    const auto literal = parse_numeric_literal_byte(arg.substr(1));
    return std::make_unique<Parser::Program::Instruction::Immediate>(
        static_cast<common::bytes::Byte>(literal));
  }

  if (arg[0] == '$' || std::isdigit(arg[0])) {
    auto literal = parse_numeric_literal(arg);
    return std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(
        static_cast<common::bytes::Word>(literal));
  }

  // Otherwise treat it as a label
  return std::make_unique<Parser::Program::Instruction::AbsoluteLabel>(arg);
}

std::unique_ptr<Parser::Program::Instruction>
Parser::Program::Instruction::parse(std::string_view line) {
  const auto tokens = common::strings::split(line, " \t");
  if (tokens.size() == 1) {
    return std::make_unique<Parser::Program::Instruction>(
        tokens[0], std::make_unique<Parser::Program::Instruction::None>());
  } else if (tokens.size() == 2) {
    return std::make_unique<Parser::Program::Instruction>(
        tokens[0], Arg::parse(tokens[1]));
  } else {
    throw std::invalid_argument("Invalid instruction: \"" + std::string(line) +
                                "\"");
  }
}

namespace {

std::unique_ptr<Parser::Program::Statement>
parse_directive(std::string_view line) {
  const auto tokens = common::strings::split(line, " \t");
  if (tokens.empty()) {
    throw std::invalid_argument("Invalid directive: \"" + std::string(line) +
                                "\"");
  }
  const auto &directive_name = tokens[0];
  if (directive_name == "byte") {
    if (tokens.size() != 2) {
      throw std::invalid_argument("Invalid directive: \"" + std::string(line) +
                                  "\": byte must have value");
    }
    return std::make_unique<Parser::Program::ByteDirective>(
        parse_numeric_literal_byte(tokens[1]));
  } else {
    throw std::invalid_argument("Invalid directive: \"" + std::string(line) +
                                "\" with unknown directive name \"" +
                                directive_name + "\"");
  }
}

} // namespace

void Parser::Program::Statement::parse(
    std::vector<std::unique_ptr<Parser::Program::Statement>> &statements,
    std::string_view untrimmed_line) {
  const std::string line = common::strings::trim(untrimmed_line);
  if (line.empty()) {
    return;
  }
  if (const auto comment_start = line.find(';');
      comment_start != std::string::npos) {
    parse(statements, line.substr(0, comment_start));
    statements.emplace_back(std::make_unique<Parser::Program::Comment>(
        common::strings::trim(line.substr(comment_start + 1))));
    return;
  }
  if (const auto label_end = line.find(':'); label_end != std::string::npos) {
    statements.emplace_back(std::make_unique<Parser::Program::Label>(
        common::strings::trim(line.substr(0, label_end))));
    return parse(statements, line.substr(label_end + 1));
  }
  if (line[0] == '.') {
    statements.push_back(parse_directive(line.substr(1)));
    return;
  }
  if (auto instruction = Instruction::parse(line); instruction != nullptr) {
    statements.push_back(std::move(instruction));
  }
}

Parser::Program Parser::Program::parse(std::string_view input) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  for (const auto &line : common::strings::split(input, "\n")) {
    Statement::parse(statements, line);
  }
  return Parser::Program(std::move(statements));
}

Parser::Program Parser::parse(std::string_view input) {
  return Program::parse(input);
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Statement::Type &type) {
  switch (type) {
  case Parser::Program::Statement::Type::Comment:
    return os << "Comment";
  case Parser::Program::Statement::Type::Label:
    return os << "Label";
  case Parser::Program::Statement::Type::Instruction:
    return os << "Instruction";
  case Parser::Program::Statement::Type::ByteDirective:
    return os << "ByteDirective";
  }
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Arg::Type &type) {
  switch (type) {
  case Parser::Program::Instruction::Arg::Type::None:
    return os << "None";
  case Parser::Program::Instruction::Arg::Type::Immediate:
    return os << "Immediate";
  case Parser::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return os << "AbsoluteLiteral";
  case Parser::Program::Instruction::Arg::Type::AbsoluteLabel:
    return os << "AbsoluteLabel";
  }
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::None &arg) {
  return os << "None()";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Immediate &arg) {
  return os << "Immediate(value = " << arg.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::AbsoluteLiteral &arg) {
  return os << "AbsoluteLiteral(value = " << arg.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::AbsoluteLabel &arg) {
  return os << "AbsoluteLabel(value = " << arg.value() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Arg &arg) {
  switch (arg.type()) {
  case Parser::Program::Instruction::Arg::Type::None:
    return os << dynamic_cast<const Parser::Program::Instruction::None &>(arg);
  case Parser::Program::Instruction::Arg::Type::Immediate:
    return os << dynamic_cast<const Parser::Program::Instruction::Immediate &>(
               arg);
  case Parser::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return os << dynamic_cast<
               const Parser::Program::Instruction::AbsoluteLiteral &>(arg);
  case Parser::Program::Instruction::Arg::Type::AbsoluteLabel:
    return os
           << dynamic_cast<const Parser::Program::Instruction::AbsoluteLabel &>(
                  arg);
  }
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Comment &comment) {
  return os << "Comment(value = " << comment.value() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Label &label) {
  return os << "Label(value = " << label.value() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction &instruction) {
  return os << "Instruction(instruction = " << instruction.instruction()
            << ", arg = " << instruction.arg() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::ByteDirective &directive) {
  return os << "ByteDirective(value = " << directive.value() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Statement &statement) {
  switch (statement.type()) {
  case Parser::Program::Statement::Type::Comment:
    return os << dynamic_cast<const Parser::Program::Comment &>(statement);
  case Parser::Program::Statement::Type::Label:
    return os << dynamic_cast<const Parser::Program::Label &>(statement);
  case Parser::Program::Statement::Type::Instruction:
    return os << dynamic_cast<const Parser::Program::Instruction &>(statement);
  case Parser::Program::Statement::Type::ByteDirective:
    return os << dynamic_cast<const Parser::Program::ByteDirective &>(
               statement);
  }
}

std::ostream &operator<<(std::ostream &os, const Parser::Program &program) {
  std::vector<std::string> statement_strs;
  for (const auto &statement : program.statements()) {
    std::ostringstream os;
    os << *statement;
    statement_strs.push_back(os.str());
  }
  return os << "Program(" << common::strings::join(statement_strs, ", ") << ")";
}

} // namespace irata::assembler
