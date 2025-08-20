#include <iostream>
#include <irata/assembler/label_binder.hpp>
#include <irata/common/strings/strings.hpp>
#include <sstream>

namespace irata::assembler {

LabelBinder::BindContext::BindContext(
    std::map<std::string, common::bytes::Word> labels)
    : labels_(std::move(labels)) {}

namespace {

std::map<std::string, common::bytes::Word>
extract_labels(const InstructionBinder::Program &program) {
  std::map<std::string, common::bytes::Word> labels;
  for (const auto &statement : program.statements()) {
    if (statement->type() ==
        InstructionBinder::Program::Statement::Type::Label) {
      const auto &label =
          dynamic_cast<const InstructionBinder::Program::Label &>(*statement);
      if (labels.find(label.value()) != labels.end()) {
        throw std::invalid_argument("duplicate label: " + label.value());
      }
      labels[label.value()] = label.address();
    }
  }
  return labels;
}

} // namespace

LabelBinder::BindContext::BindContext(const InstructionBinder::Program &program)
    : BindContext(extract_labels(program)) {}

bool LabelBinder::BindContext::contains(std::string_view label) const {
  return labels_.find(std::string(label)) != labels_.end();
}

common::bytes::Word
LabelBinder::BindContext::get(std::string_view label) const {
  if (const auto it = labels_.find(std::string(label)); it != labels_.end()) {
    return it->second;
  } else {
    throw std::invalid_argument("label not found: " + std::string(label));
  }
}

LabelBinder::Program::Statement::Statement(Type type,
                                           common::bytes::Word address)
    : type_(type), address_(address) {}

std::unique_ptr<LabelBinder::Program::Statement>
LabelBinder::Program::Statement::bind(
    const InstructionBinder::Program::Statement &statement,
    const BindContext &context) {
  switch (statement.type()) {
  case InstructionBinder::Program::Statement::Type::Label:
    return nullptr;
  case InstructionBinder::Program::Statement::Type::Instruction:
    return std::make_unique<Instruction>(
        dynamic_cast<const InstructionBinder::Program::Instruction &>(
            statement),
        context);
  case InstructionBinder::Program::Statement::Type::Literal:
    return std::make_unique<Literal>(
        statement.address(),
        dynamic_cast<const InstructionBinder::Program::Literal &>(statement));
  }
}

LabelBinder::Program::Statement::Type
LabelBinder::Program::Statement::type() const {
  return type_;
}

common::bytes::Word LabelBinder::Program::Statement::address() const {
  return address_;
}

bool LabelBinder::Program::Statement::operator==(const Statement &other) const {
  return type_ == other.type_ && address_ == other.address_;
}

bool LabelBinder::Program::Statement::operator!=(const Statement &other) const {
  return !(*this == other);
}

LabelBinder::Program::Instruction::Arg::Arg(Type type) : type_(type) {}

std::unique_ptr<LabelBinder::Program::Instruction::Arg>
LabelBinder::Program::Instruction::Arg::bind(
    const InstructionBinder::Program::Instruction::Arg &arg,
    const BindContext &context) {
  switch (arg.type()) {
  case InstructionBinder::Program::Instruction::Arg::Type::None:
    return std::make_unique<None>();
  case InstructionBinder::Program::Instruction::Arg::Type::Immediate:
    return std::make_unique<Immediate>(
        dynamic_cast<
            const InstructionBinder::Program::Instruction::Immediate &>(arg));
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLiteral:
    return std::make_unique<Absolute>(
        dynamic_cast<
            const InstructionBinder::Program::Instruction::AbsoluteLiteral &>(
            arg));
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLabel:
    return std::make_unique<Absolute>(
        dynamic_cast<
            const InstructionBinder::Program::Instruction::AbsoluteLabel &>(
            arg),
        context);
  case InstructionBinder::Program::Instruction::Arg::Type::ZeroPageIndexed:
    return std::make_unique<ZeroPageIndexed>(
        dynamic_cast<
            const InstructionBinder::Program::Instruction::ZeroPageIndexed &>(
            arg));
  case InstructionBinder::Program::Instruction::Arg::Type::AbsoluteIndexed:
    return std::make_unique<AbsoluteIndexed>(
        dynamic_cast<
            const InstructionBinder::Program::Instruction::AbsoluteIndexed &>(
            arg));
  }
}

LabelBinder::Program::Instruction::Arg::Type
LabelBinder::Program::Instruction::Arg::type() const {
  return type_;
}

bool LabelBinder::Program::Instruction::Arg::operator==(
    const Arg &other) const {
  return type_ == other.type_;
}

bool LabelBinder::Program::Instruction::Arg::operator!=(
    const Arg &other) const {
  return !(*this == other);
}

LabelBinder::Program::Instruction::None::None() : Arg(Type::None) {}

LabelBinder::Program::Instruction::Immediate::Immediate(
    common::bytes::Byte value)
    : Arg(Type::Immediate), value_(value) {}

LabelBinder::Program::Instruction::Immediate::Immediate(
    const InstructionBinder::Program::Instruction::Immediate &arg)
    : Immediate(arg.value()) {}

common::bytes::Byte
LabelBinder::Program::Instruction::Immediate::value() const {
  return value_;
}

bool LabelBinder::Program::Instruction::Immediate::operator==(
    const Arg &other) const {
  return Arg::operator==(other) &&
         value_ == dynamic_cast<const Immediate &>(other).value_;
}

LabelBinder::Program::Instruction::Absolute::Absolute(common::bytes::Word value)
    : Arg(Type::Absolute), value_(value) {}

LabelBinder::Program::Instruction::Absolute::Absolute(
    const InstructionBinder::Program::Instruction::AbsoluteLiteral &arg)
    : Absolute(arg.value()) {}

// Note 0x8000 offset to account for cartridge memory mapping.
LabelBinder::Program::Instruction::Absolute::Absolute(
    const InstructionBinder::Program::Instruction::AbsoluteLabel &arg,
    const BindContext &context)
    : Absolute(context.get(arg.value()) + common::bytes::Word(0x8000)) {}

common::bytes::Word LabelBinder::Program::Instruction::Absolute::value() const {
  return value_;
}

bool LabelBinder::Program::Instruction::Absolute::operator==(
    const Arg &other) const {
  return Arg::operator==(other) &&
         value_ == dynamic_cast<const Absolute &>(other).value_;
}

LabelBinder::Program::Instruction::ZeroPageIndexed::ZeroPageIndexed(
    Index index, common::bytes::Byte value)
    : Arg(Type::ZeroPageIndexed), index_(index), value_(value) {}

LabelBinder::Program::Instruction::ZeroPageIndexed::ZeroPageIndexed(
    const InstructionBinder::Program::Instruction::ZeroPageIndexed &arg)
    : ZeroPageIndexed(arg.index(), arg.value()) {}

Index LabelBinder::Program::Instruction::ZeroPageIndexed::index() const {
  return index_;
}

common::bytes::Byte
LabelBinder::Program::Instruction::ZeroPageIndexed::value() const {
  return value_;
}

bool LabelBinder::Program::Instruction::ZeroPageIndexed::operator==(
    const Arg &other) const {
  if (!Arg::operator==(other)) {
    return false;
  }
  const auto &rhs = dynamic_cast<const ZeroPageIndexed &>(other);
  return index_ == rhs.index_ && value_ == rhs.value_;
}

LabelBinder::Program::Instruction::AbsoluteIndexed ::AbsoluteIndexed(
    Index index, common::bytes::Word value)
    : Arg(Type::AbsoluteIndexed), index_(index), value_(value) {}

LabelBinder::Program::Instruction::AbsoluteIndexed ::AbsoluteIndexed(
    const InstructionBinder::Program::Instruction::AbsoluteIndexed &arg)
    : AbsoluteIndexed(arg.index(), arg.value()) {}

Index LabelBinder::Program::Instruction::AbsoluteIndexed::index() const {
  return index_;
}

common::bytes::Word
LabelBinder::Program::Instruction::AbsoluteIndexed::value() const {
  return value_;
}

bool LabelBinder::Program::Instruction::AbsoluteIndexed::operator==(
    const Arg &other) const {
  if (!Arg::operator==(other)) {
    return false;
  }
  const auto &rhs = dynamic_cast<const AbsoluteIndexed &>(other);
  return index_ == rhs.index_ && value_ == rhs.value_;
}

LabelBinder::Program::Instruction::Instruction(
    common::bytes::Word address, const asm_::Instruction &instruction,
    std::unique_ptr<Arg> arg)
    : Statement(Type::Instruction, address), instruction_(instruction),
      arg_(std::move(arg)) {
  if (arg_ == nullptr) {
    throw std::invalid_argument("null arg");
  }
}

LabelBinder::Program::Instruction::Instruction(
    const InstructionBinder::Program::Instruction &instruction,
    const BindContext &context)
    : Instruction(instruction.address(), instruction.instruction(),
                  Arg::bind(instruction.arg(), context)) {}

const asm_::Instruction &
LabelBinder::Program::Instruction::instruction() const {
  return instruction_;
}

const LabelBinder::Program::Instruction::Arg &
LabelBinder::Program::Instruction::arg() const {
  return *arg_;
}

bool LabelBinder::Program::Instruction::operator==(
    const Statement &other) const {
  if (!Statement::operator==(other)) {
    return false;
  }
  const auto &other_instruction = dynamic_cast<const Instruction &>(other);
  return instruction_ == other_instruction.instruction_ &&
         *arg_ == *other_instruction.arg_;
}

LabelBinder::Program::Literal::Literal(common::bytes::Word address,
                                       std::vector<common::bytes::Byte> values)
    : Statement(Type::Literal, address), values_(std::move(values)) {}

LabelBinder::Program::Literal::Literal(
    common::bytes::Word address,
    const InstructionBinder::Program::Literal &literal)
    : Literal(address, literal.values()) {}

bool LabelBinder::Program::Literal::operator==(const Statement &other) const {
  return Statement::operator==(other) &&
         values_ == dynamic_cast<const Literal &>(other).values_;
}

const std::vector<common::bytes::Byte> &
LabelBinder::Program::Literal::values() const {
  return values_;
}

LabelBinder::Program::Program(
    std::vector<std::unique_ptr<Statement>> statements)
    : statements_(std::move(statements)) {
  for (const auto &statement : statements_) {
    if (statement == nullptr) {
      throw std::invalid_argument("null statement");
    }
  }
}

const std::vector<std::unique_ptr<LabelBinder::Program::Statement>> &
LabelBinder::Program::statements() const {
  return statements_;
}

bool LabelBinder::Program::operator==(const Program &other) const {
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

bool LabelBinder::Program::operator!=(const Program &other) const {
  return !(*this == other);
}

LabelBinder::Program
LabelBinder::bind(const InstructionBinder::Program &program) {
  const BindContext context(program);
  std::vector<std::unique_ptr<Program::Statement>> statements;
  for (const auto &statement : program.statements()) {
    if (auto bound_statement = Program::Statement::bind(*statement, context);
        bound_statement != nullptr) {
      statements.push_back(std::move(bound_statement));
    }
  }
  return Program(std::move(statements));
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Statement::Type &type) {
  switch (type) {
  case LabelBinder::Program::Statement::Type::Instruction:
    return os << "Instruction";
  case LabelBinder::Program::Statement::Type::Literal:
    return os << "Literal";
  }
}

std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Arg::Type &type) {
  switch (type) {
  case LabelBinder::Program::Instruction::Arg::Type::None:
    return os << "None";
  case LabelBinder::Program::Instruction::Arg::Type::Immediate:
    return os << "Immediate";
  case LabelBinder::Program::Instruction::Arg::Type::Absolute:
    return os << "Absolute";
  case LabelBinder::Program::Instruction::Arg::Type::ZeroPageIndexed:
    return os << "ZeroPageIndexed";
  case LabelBinder::Program::Instruction::Arg::Type::AbsoluteIndexed:
    return os << "AbsoluteIndexed";
  }
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction::None &none) {
  return os << "None()";
}

std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Immediate &immediate) {
  return os << "Immediate(value = " << immediate.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Absolute &absolute) {
  return os << "Absolute(value = " << absolute.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::ZeroPageIndexed &arg) {
  return os << "ZeroPageIndexed(index = " << arg.index()
            << ", value = " << arg.value() << ")";
}

std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::AbsoluteIndexed &arg) {
  return os << "AbsoluteIndexed(index = " << arg.index()
            << ", value = " << arg.value() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction::Arg &arg) {
  switch (arg.type()) {
  case LabelBinder::Program::Instruction::Arg::Type::None:
    return os << dynamic_cast<const LabelBinder::Program::Instruction::None &>(
               arg);
  case LabelBinder::Program::Instruction::Arg::Type::Immediate:
    return os << dynamic_cast<
               const LabelBinder::Program::Instruction::Immediate &>(arg);
  case LabelBinder::Program::Instruction::Arg::Type::Absolute:
    return os
           << dynamic_cast<const LabelBinder::Program::Instruction::Absolute &>(
                  arg);
  case LabelBinder::Program::Instruction::Arg::Type::ZeroPageIndexed:
    return os << dynamic_cast<
               const LabelBinder::Program::Instruction::ZeroPageIndexed &>(arg);
  case LabelBinder::Program::Instruction::Arg::Type::AbsoluteIndexed:
    return os << dynamic_cast<
               const LabelBinder::Program::Instruction::AbsoluteIndexed &>(arg);
  }
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction &instruction) {
  return os << "Instruction(instruction = " << instruction.instruction()
            << ", arg = " << instruction.arg() << ")";
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Literal &literal) {
  std::vector<std::string> value_strs;
  for (const auto &value : literal.values()) {
    std::ostringstream os;
    os << value;
    value_strs.push_back(os.str());
  }
  return os << "Literal(address = " << literal.address() << ", values = ["
            << common::strings::join(value_strs, ", ") << "])";
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Statement &statement) {
  switch (statement.type()) {
  case LabelBinder::Program::Statement::Type::Instruction:
    return os << dynamic_cast<const LabelBinder::Program::Instruction &>(
               statement);
  case LabelBinder::Program::Statement::Type::Literal:
    return os << dynamic_cast<const LabelBinder::Program::Literal &>(statement);
  }
}

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program &program) {
  std::vector<std::string> statement_strs;
  for (const auto &statement : program.statements()) {
    std::ostringstream os;
    os << *statement;
    statement_strs.push_back(os.str());
  }
  return os << "Program(" << common::strings::join(statement_strs, ", ") << ")";
}

} // namespace irata::assembler
