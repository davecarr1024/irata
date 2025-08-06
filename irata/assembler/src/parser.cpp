#include <cctype>
#include <charconv>
#include <irata/assembler/parser.hpp>
#include <irata/common/strings/strings.hpp>
#include <stdexcept>

namespace irata::assembler {

bool Parser::Program::Statement::operator==(const Statement &other) const {
  return type() == other.type();
}

bool Parser::Program::Statement::operator!=(const Statement &other) const {
  return !(*this == other);
}

Parser::Program::StatementWithValue::StatementWithValue(std::string_view value)
    : value_(value) {}

std::string Parser::Program::StatementWithValue::value() const {
  return value_;
}

bool Parser::Program::StatementWithValue::operator==(
    const Statement &other) const {
  return Statement::operator==(other) &&
         value_ == dynamic_cast<const StatementWithValue &>(other).value_;
}

Parser::Program::Comment::Comment(std::string_view value)
    : StatementWithValue(value) {}

Parser::Program::Label::Label(std::string_view value)
    : StatementWithValue(value) {}

bool Parser::Program::Instruction::Arg::operator==(const Arg &other) const {
  return type() == other.type() && addressing_mode() == other.addressing_mode();
}

bool Parser::Program::Instruction::Arg::operator!=(const Arg &other) const {
  return !(*this == other);
}

Parser::Program::Instruction::Immediate::Immediate(common::bytes::Byte value)
    : ArgWithValue<common::bytes::Byte>(value) {}

bool Parser::Program::Instruction::Immediate::operator==(
    const Arg &other) const {
  return ArgWithValue<common::bytes::Byte>::operator==(other);
}

Parser::Program::Instruction::AbsoluteLiteral::AbsoluteLiteral(
    common::bytes::Word value)
    : ArgWithValue<common::bytes::Word>(value) {}

bool Parser::Program::Instruction::AbsoluteLiteral::operator==(
    const Arg &other) const {
  return ArgWithValue<common::bytes::Word>::operator==(other);
}

Parser::Program::Instruction::AbsoluteLabel::AbsoluteLabel(
    std::string_view value)
    : ArgWithValue<std::string>(std::string(value)) {}

bool Parser::Program::Instruction::AbsoluteLabel::operator==(
    const Arg &other) const {
  return ArgWithValue<std::string>::operator==(other);
}

Parser::Program::Instruction::Instruction(std::string_view instruction,
                                          std::unique_ptr<Arg> arg)
    : instruction_(instruction), arg_(std::move(arg)) {
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

  int base = 10;
  if (value[0] == '$') {
    base = 16;
    value.remove_prefix(1);
  } else if (common::strings::starts_with(value, "0x") ||
             common::strings::starts_with(value, "0X")) {
    base = 16;
    value.remove_prefix(2);
  }

  uint16_t result = 0;
  auto [ptr, ec] =
      std::from_chars(value.data(), value.data() + value.size(), result, base);
  if (ec != std::errc()) {
    throw std::invalid_argument("Invalid numeric format: " +
                                std::string(value));
  }
  return result;
}

std::unique_ptr<Parser::Program::Instruction::Arg>
parse_arg(const std::string &arg) {
  if (arg.empty()) {
    return std::make_unique<Parser::Program::Instruction::None>();
  }

  if (arg[0] == '#') {
    auto literal = parse_numeric_literal(arg.substr(1));
    if (literal > 0xFF) {
      throw std::invalid_argument("Immediate value out of 8-bit range");
    }
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

void parse_line(
    const std::string &line,
    std::vector<std::unique_ptr<Parser::Program::Statement>> &statements) {
  if (const auto comment_start = line.find(';');
      comment_start != std::string::npos) {
    parse_line(line.substr(0, comment_start), statements);
    statements.emplace_back(std::make_unique<Parser::Program::Comment>(
        common::strings::trim(line.substr(comment_start + 1))));
    return;
  }
  if (const auto label_end = line.find(':'); label_end != std::string::npos) {
    statements.emplace_back(std::make_unique<Parser::Program::Label>(
        common::strings::trim(line.substr(0, label_end))));
    return parse_line(line.substr(label_end + 1), statements);
  }
  const auto tokens = common::strings::split(line, " \t");
  if (tokens.size() == 1) {
    statements.emplace_back(std::make_unique<Parser::Program::Instruction>(
        tokens[0], std::make_unique<Parser::Program::Instruction::None>()));
  }
  if (tokens.size() == 2) {
    const auto &instruction = tokens[0];
    const auto &arg = tokens[1];
    statements.emplace_back(std::make_unique<Parser::Program::Instruction>(
        instruction, parse_arg(arg)));
  }
}

void parse_lines(
    std::string_view input,
    std::vector<std::unique_ptr<Parser::Program::Statement>> &statements) {
  for (const auto &line : common::strings::split(input, "\n")) {
    parse_line(line, statements);
  }
}

} // namespace

Parser::Program Parser::parse(std::string_view input) {
  std::vector<std::unique_ptr<Program::Statement>> statements;
  parse_lines(input, statements);
  return Program(std::move(statements));
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
  default:
    throw std::logic_error("Unknown statement type");
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
  default:
    throw std::logic_error("Unknown arg type");
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
  default:
    throw std::logic_error("Unknown arg type");
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
                         const Parser::Program::Statement &statement) {
  switch (statement.type()) {
  case Parser::Program::Statement::Type::Comment:
    return os << dynamic_cast<const Parser::Program::Comment &>(statement);
  case Parser::Program::Statement::Type::Label:
    return os << dynamic_cast<const Parser::Program::Label &>(statement);
  case Parser::Program::Statement::Type::Instruction:
    return os << dynamic_cast<const Parser::Program::Instruction &>(statement);
  default:
    throw std::logic_error("Unknown statement type");
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
