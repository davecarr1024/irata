#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/instruction_binder.hpp>
#include <irata/common/strings/strings.hpp>
#include <stdexcept>

namespace irata::assembler {

InstructionBinder::Program::Statement::Statement(Type type,
                                                 common::bytes::Word address)
    : type_(type), address_(address) {}

std::unique_ptr<InstructionBinder::Program::Statement>
InstructionBinder::Program::Statement::bind(
    common::bytes::Word address, const Parser::Program::Statement &statement) {
  switch (statement.type()) {
  case Parser::Program::Statement::Type::Comment:
    return nullptr;
  case Parser::Program::Statement::Type::Label:
    return std::make_unique<Label>(
        address, dynamic_cast<const Parser::Program::Label &>(statement));
  case Parser::Program::Statement::Type::Instruction:
    return std::make_unique<Instruction>(
        address, dynamic_cast<const Parser::Program::Instruction &>(statement));
  case Parser::Program::Statement::Type::ByteDirective:
    return std::make_unique<Literal>(
        address,
        dynamic_cast<const Parser::Program::ByteDirective &>(statement));
  }
}

InstructionBinder::Program::Statement::Type
InstructionBinder::Program::Statement::type() const {
  return type_;
}

common::bytes::Word InstructionBinder::Program::Statement::address() const {
  return address_;
}

bool InstructionBinder::Program::Statement::operator==(
    const Statement &other) const {
  return type_ == other.type_ && address_ == other.address_ &&
         size() == other.size();
}

bool InstructionBinder::Program::Statement::operator!=(
    const Statement &other) const {
  return !(*this == other);
}

InstructionBinder::Program::Label::Label(common::bytes::Word address,
                                         std::string_view value)
    : Statement(Type::Label, address), value_(value) {}

InstructionBinder::Program::Label::Label(common::bytes::Word address,
                                         const Parser::Program::Label &label)
    : Label(address, label.value()) {}

std::string InstructionBinder::Program::Label::value() const { return value_; }

size_t InstructionBinder::Program::Label::size() const { return 0; }

bool InstructionBinder::Program::Label::operator==(
    const Statement &other) const {
  return Statement::operator==(other) &&
         value_ == dynamic_cast<const Label &>(other).value_;
}

InstructionBinder::Program::Instruction::Arg::Arg(Type type) : type_(type) {}

std::unique_ptr<InstructionBinder::Program::Instruction::Arg>
InstructionBinder::Program::Instruction::Arg::bind(
    const Parser::Program::Instruction::Arg &arg) {
  switch (arg.type()) {
  case Parser::Program::Instruction::Arg::Type::None:
    return std::make_unique<None>();
  case Parser::Program::Instruction::Arg::Type::Immediate:
    return std::make_unique<Immediate>(
        dynamic_cast<const Parser::Program::Instruction::Immediate &>(arg));
  case Parser::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return std::make_unique<AbsoluteLiteral>(
        dynamic_cast<const Parser::Program::Instruction::AbsoluteLiteral &>(
            arg));
  case Parser::Program::Instruction::Arg::Type::AbsoluteLabel:
    return std::make_unique<AbsoluteLabel>(
        dynamic_cast<const Parser::Program::Instruction::AbsoluteLabel &>(arg));
  }
}

InstructionBinder::Program::Instruction::Arg::Type
InstructionBinder::Program::Instruction::Arg::type() const {
  return type_;
}

bool InstructionBinder::Program::Instruction::Arg::operator==(
    const Arg &other) const {
  return type_ == other.type_ && size() == other.size();
}

bool InstructionBinder::Program::Instruction::Arg::operator!=(
    const Arg &other) const {
  return !(*this == other);
}

InstructionBinder::Program::Instruction::None::None() : Arg(Type::None) {}

size_t InstructionBinder::Program::Instruction::None::size() const { return 0; }

InstructionBinder::Program::Instruction::Immediate::Immediate(
    common::bytes::Byte value)
    : Arg(Type::Immediate), value_(value) {}

InstructionBinder::Program::Instruction::Immediate::Immediate(
    const Parser::Program::Instruction::Immediate &arg)
    : Immediate(arg.value()) {}

common::bytes::Byte
InstructionBinder::Program::Instruction::Immediate::value() const {
  return value_;
}

size_t InstructionBinder::Program::Instruction::Immediate::size() const {
  return 1;
}

bool InstructionBinder::Program::Instruction::Immediate::operator==(
    const Arg &other) const {
  return Arg::operator==(other) &&
         value_ == dynamic_cast<const Immediate &>(other).value_;
}

InstructionBinder::Program::Instruction::AbsoluteLiteral::AbsoluteLiteral(
    common::bytes::Word value)
    : Arg(Type::AbsoluteLiteral), value_(value) {}

InstructionBinder::Program::Instruction::AbsoluteLiteral::AbsoluteLiteral(
    const Parser::Program::Instruction::AbsoluteLiteral &arg)
    : AbsoluteLiteral(arg.value()) {}

common::bytes::Word
InstructionBinder::Program::Instruction::AbsoluteLiteral::value() const {
  return value_;
}

size_t InstructionBinder::Program::Instruction::AbsoluteLiteral::size() const {
  return 2;
}

bool InstructionBinder::Program::Instruction::AbsoluteLiteral::operator==(
    const Arg &other) const {
  return Arg::operator==(other) &&
         value_ == dynamic_cast<const AbsoluteLiteral &>(other).value_;
}

InstructionBinder::Program::Instruction::AbsoluteLabel::AbsoluteLabel(
    std::string_view value)
    : Arg(Type::AbsoluteLabel), value_(value) {}

InstructionBinder::Program::Instruction::AbsoluteLabel::AbsoluteLabel(
    const Parser::Program::Instruction::AbsoluteLabel &arg)
    : AbsoluteLabel(arg.value()) {}

std::string
InstructionBinder::Program::Instruction::AbsoluteLabel::value() const {
  return value_;
}

size_t InstructionBinder::Program::Instruction::AbsoluteLabel::size() const {
  return 2;
}

bool InstructionBinder::Program::Instruction::AbsoluteLabel::operator==(
    const Arg &other) const {
  return Arg::operator==(other) &&
         value_ == dynamic_cast<const AbsoluteLabel &>(other).value_;
}

InstructionBinder::Program::Instruction::Instruction(
    common::bytes::Word address, const asm_::Instruction &instruction,
    std::unique_ptr<Arg> arg)
    : Statement(Type::Instruction, address), instruction_(instruction),
      arg_(std::move(arg)) {
  if (arg_ == nullptr) {
    throw std::invalid_argument("null arg");
  }
}

InstructionBinder::Program::Instruction::Instruction(
    common::bytes::Word address,
    const Parser::Program::Instruction &instruction)
    : Instruction(
          address,
          asm_::InstructionSet::irata().get_instruction(
              instruction.instruction(), instruction.arg().addressing_mode()),
          Arg::bind(instruction.arg())) {}

const asm_::Instruction &
InstructionBinder::Program::Instruction::instruction() const {
  return instruction_;
}

const InstructionBinder::Program::Instruction::Arg &
InstructionBinder::Program::Instruction::arg() const {
  return *arg_;
}

size_t InstructionBinder::Program::Instruction::size() const {
  return 1 + arg_->size();
}

bool InstructionBinder::Program::Instruction::operator==(
    const Statement &other) const {
  if (!Statement::operator==(other)) {
    return false;
  }
  const auto &other_instruction = dynamic_cast<const Instruction &>(other);
  return instruction_ == other_instruction.instruction_ &&
         *arg_ == *other_instruction.arg_;
}

InstructionBinder::Program::Literal::Literal(
    common::bytes::Word address, std::vector<common::bytes::Byte> values)
    : Statement(Type::Literal, address), values_(std::move(values)) {}

InstructionBinder::Program::Literal::Literal(
    common::bytes::Word address,
    const Parser::Program::ByteDirective &directive)
    : Literal(address, {directive.value()}) {}

size_t InstructionBinder::Program::Literal::size() const {
  return values_.size();
}

bool InstructionBinder::Program::Literal::operator==(
    const Statement &other) const {
  return Statement::operator==(other) &&
         values_ == dynamic_cast<const Literal &>(other).values_;
}

const std::vector<common::bytes::Byte> &
InstructionBinder::Program::Literal::values() const {
  return values_;
}

InstructionBinder::Program::Program(
    std::vector<std::unique_ptr<Statement>> statements)
    : statements_(std::move(statements)) {
  for (const auto &statement : statements_) {
    if (statement == nullptr) {
      throw std::invalid_argument("null statement");
    }
  }
}

const std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> &
InstructionBinder::Program::statements() const {
  return statements_;
}

bool InstructionBinder::Program::operator==(const Program &other) const {
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

bool InstructionBinder::Program::operator!=(const Program &other) const {
  return !(*this == other);
}

InstructionBinder::Program
InstructionBinder::bind(const Parser::Program &program) {
  std::vector<std::unique_ptr<Program::Statement>> statements;
  common::bytes::Word address = 0x0000;
  for (const auto &parser_statement : program.statements()) {
    if (auto statement = Program::Statement::bind(address, *parser_statement);
        statement != nullptr) {
      address = address + statement->size();
      statements.push_back(std::move(statement));
    }
  }
  return Program(std::move(statements));
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Statement::Type &type) {
  switch (type) {
  case InstructionBinder::Program::Statement::Type::Label:
    return os << "Label";
  case InstructionBinder::Program::Statement::Type::Instruction:
    return os << "Instruction";
  case InstructionBinder::Program::Statement::Type::Literal:
    return os << "Literal";
  }
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Arg::Type &type) {
  switch (type) {
  case InstructionBinder::Program::Instruction::Arg::Type::None:
    return os << "None";
  case InstructionBinder::Program::Instruction::Arg::Type::Immediate:
    return os << "Immediate";
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return os << "AbsoluteLiteral";
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLabel:
    return os << "AbsoluteLabel";
  }
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::None &arg) {
  return os << "None()";
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Immediate &arg) {
  return os << "Immediate(value = " << arg.value() << ")";
}

std::ostream &operator<<(
    std::ostream &os,
    const InstructionBinder::Program::Instruction::AbsoluteLiteral &arg) {
  return os << "AbsoluteLiteral(value = " << arg.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::AbsoluteLabel &arg) {
  return os << "AbsoluteLabel(value = " << arg.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Arg &arg) {
  switch (arg.type()) {
  case InstructionBinder::Program::Instruction::Arg::Type::None:
    return os << dynamic_cast<
               const InstructionBinder::Program::Instruction::None &>(arg);
  case InstructionBinder::Program::Instruction::Arg::Type::Immediate:
    return os << dynamic_cast<
               const InstructionBinder::Program::Instruction::Immediate &>(arg);
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return os << dynamic_cast<const InstructionBinder::Program::Instruction::
                                  AbsoluteLiteral &>(arg);
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLabel:
    return os << dynamic_cast<
               const InstructionBinder::Program::Instruction::AbsoluteLabel &>(
               arg);
  }
}

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program::Label &label) {
  return os << "Label(address = " << label.address()
            << ", value = " << label.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction &instruction) {
  return os << "Instruction(instruction = " << instruction.instruction()
            << ", arg = " << instruction.arg() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program::Literal &literal) {
  std::vector<std::string> value_strs;
  for (const auto &value : literal.values()) {
    std::ostringstream os;
    os << value;
    value_strs.push_back(os.str());
  }
  return os << "Literal(address = " << literal.address() << ", values = ["
            << common::strings::join(value_strs, ", ") << "])";
}

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Statement &statement) {
  switch (statement.type()) {
  case InstructionBinder::Program::Statement::Type::Label:
    return os << dynamic_cast<const InstructionBinder::Program::Label &>(
               statement);
  case InstructionBinder::Program::Statement::Type::Instruction:
    return os << dynamic_cast<const InstructionBinder::Program::Instruction &>(
               statement);
  case InstructionBinder::Program::Statement::Type::Literal:
    return os << dynamic_cast<const InstructionBinder::Program::Literal &>(
               statement);
  }
}

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program &program) {
  std::vector<std::string> statement_strs;
  for (const auto &statement : program.statements()) {
    std::ostringstream os;
    os << *statement;
    statement_strs.push_back(os.str());
  }
  return os << "Program(" << common::strings::join(statement_strs, ", ") << ")";
}

} // namespace irata::assembler
