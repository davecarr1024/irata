#pragma once

#include <irata/assembler/instruction_binder.hpp>
#include <irata/assembler/source_location.hpp>
#include <map>

namespace irata::assembler {

class LabelBinder final {
private:
  class BindContext final {
  public:
    explicit BindContext(std::map<std::string, common::bytes::Word> labels);
    explicit BindContext(const InstructionBinder::Program &program);

    bool contains(std::string_view label) const;

    common::bytes::Word get(std::string_view label) const;

    const std::map<std::string, common::bytes::Word> &labels() const;

  private:
    const std::map<std::string, common::bytes::Word> labels_;
  };

public:
  class Program final {
  public:
    class Statement {
    public:
      enum class Type {
        Label,
        Instruction,
        Literal,
      };

      virtual ~Statement() = default;

      static std::unique_ptr<Statement>
      bind(const InstructionBinder::Program::Statement &statement,
           const BindContext &context);

      Type type() const;

      common::bytes::Word address() const;

      const SourceLocation &source_location() const;

      virtual bool operator==(const Statement &other) const;
      virtual bool operator!=(const Statement &other) const;

    protected:
      Statement(Type type, common::bytes::Word address,
                SourceLocation source_location);

    private:
      const Type type_;
      const common::bytes::Word address_;
      const SourceLocation source_location_;
    };

    class Label final : public Statement {
    public:
      Label(common::bytes::Word address, std::string value,
            SourceLocation source_location);
      Label(const InstructionBinder::Program::Label &label);

      const std::string &value() const;

      bool operator==(const Statement &other) const override final;

    private:
      const std::string value_;
    };

    class Instruction final : public Statement {
    public:
      class Arg {
      public:
        enum class Type {
          None,
          Immediate,
          Absolute,
          ZeroPageIndexed,
          AbsoluteIndexed,
        };

        virtual ~Arg() = default;

        static std::unique_ptr<Arg>
        bind(const InstructionBinder::Program::Instruction::Arg &arg,
             const BindContext &context);

        Type type() const;

        virtual bool operator==(const Arg &other) const;
        virtual bool operator!=(const Arg &other) const;

      protected:
        explicit Arg(Type type);

      private:
        const Type type_;
      };

      class None final : public Arg {
      public:
        None();
      };

      class Immediate final : public Arg {
      public:
        explicit Immediate(common::bytes::Byte value);
        explicit Immediate(
            const InstructionBinder::Program::Instruction::Immediate &arg);

        common::bytes::Byte value() const;

        bool operator==(const Arg &other) const override final;

      private:
        const common::bytes::Byte value_;
      };

      class Absolute final : public Arg {
      public:
        explicit Absolute(common::bytes::Word value);
        explicit Absolute(
            const InstructionBinder::Program::Instruction::AbsoluteLiteral
                &arg);
        Absolute(
            const InstructionBinder::Program::Instruction::AbsoluteLabel &arg,
            const BindContext &context);

        common::bytes::Word value() const;

        bool operator==(const Arg &other) const override final;

      private:
        const common::bytes::Word value_;
      };

      class ZeroPageIndexed final : public Arg {
      public:
        ZeroPageIndexed(Index index, common::bytes::Byte value);
        ZeroPageIndexed(
            const InstructionBinder::Program::Instruction::ZeroPageIndexed
                &arg);

        Index index() const;
        common::bytes::Byte value() const;

        bool operator==(const Arg &other) const override final;

      private:
        const Index index_;
        const common::bytes::Byte value_;
      };

      class AbsoluteIndexed final : public Arg {
      public:
        AbsoluteIndexed(Index index, common::bytes::Word value);
        AbsoluteIndexed(
            const InstructionBinder::Program::Instruction::AbsoluteIndexed
                &arg);

        Index index() const;
        common::bytes::Word value() const;

        bool operator==(const Arg &other) const override final;

      private:
        const Index index_;
        const common::bytes::Word value_;
      };

      Instruction(common::bytes::Word address,
                  const asm_::Instruction &instruction,
                  std::unique_ptr<Arg> arg, SourceLocation source_location);
      Instruction(const InstructionBinder::Program::Instruction &instruction,
                  const BindContext &context);

      const asm_::Instruction &instruction() const;

      const Arg &arg() const;

      bool operator==(const Statement &other) const override final;

    private:
      const asm_::Instruction &instruction_;
      const std::unique_ptr<Arg> arg_;
    };

    // A literal is a set of bytes that are inserted directly into the output.
    class Literal final : public Statement {
    public:
      Literal(common::bytes::Word address,
              std::vector<common::bytes::Byte> values,
              SourceLocation source_location);
      Literal(const InstructionBinder::Program::Literal &literal);

      const std::vector<common::bytes::Byte> &values() const;

      bool operator==(const Statement &other) const override final;

    private:
      const std::vector<common::bytes::Byte> values_;
    };

    explicit Program(std::vector<std::unique_ptr<Statement>> statements);

    const std::vector<std::unique_ptr<Statement>> &statements() const;

    bool operator==(const Program &other) const;
    bool operator!=(const Program &other) const;

  private:
    const std::vector<std::unique_ptr<Statement>> statements_;
  };

  Program bind(const InstructionBinder::Program &program);
};

std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Statement::Type &type);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Label &label);
std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Arg::Type &type);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction::None &arg);
std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Immediate &arg);
std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::Absolute &arg);
std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::ZeroPageIndexed &arg);
std::ostream &
operator<<(std::ostream &os,
           const LabelBinder::Program::Instruction::AbsoluteIndexed &arg);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction::Arg &arg);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Instruction &instruction);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Literal &literal);
std::ostream &operator<<(std::ostream &os,
                         const LabelBinder::Program::Statement &statement);
std::ostream &operator<<(std::ostream &os, const LabelBinder::Program &program);

} // namespace irata::assembler
