#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/assembler/parser.hpp>
#include <irata/common/bytes/word.hpp>

namespace irata::assembler {

class InstructionBinder final {
public:
  class Program {
  public:
    class Statement {
    public:
      enum class Type {
        Label,
        Instruction,
        Literal,
      };

      static std::unique_ptr<Statement>
      bind(common::bytes::Word address,
           const Parser::Program::Statement &statement);

      virtual ~Statement() = default;

      Type type() const;

      common::bytes::Word address() const;

      virtual size_t size() const = 0;

      virtual bool operator==(const Statement &other) const;
      virtual bool operator!=(const Statement &other) const;

    protected:
      Statement(Type type, common::bytes::Word address);

    private:
      const Type type_;
      const common::bytes::Word address_;
    };

    class Label final : public Statement {
    public:
      Label(common::bytes::Word address, std::string_view value);
      Label(common::bytes::Word address, const Parser::Program::Label &label);

      std::string value() const;

      size_t size() const override final;

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
          AbsoluteLiteral,
          AbsoluteLabel,
          ZeroPageIndexed,
        };

        virtual ~Arg() = default;

        Type type() const;

        virtual size_t size() const = 0;

        virtual bool operator==(const Arg &other) const;
        virtual bool operator!=(const Arg &other) const;

        static std::unique_ptr<Arg>
        bind(const Parser::Program::Instruction::Arg &arg);

      protected:
        explicit Arg(Type type);

      private:
        const Type type_;
      };

      class None final : public Arg {
      public:
        None();

        size_t size() const override final;
      };

      class Immediate final : public Arg {
      public:
        explicit Immediate(common::bytes::Byte value);
        explicit Immediate(const Parser::Program::Instruction::Immediate &arg);

        common::bytes::Byte value() const;

        size_t size() const override final;

        bool operator==(const Arg &other) const override final;

      private:
        const common::bytes::Byte value_;
      };

      class AbsoluteLiteral final : public Arg {
      public:
        explicit AbsoluteLiteral(common::bytes::Word value);
        explicit AbsoluteLiteral(
            const Parser::Program::Instruction::AbsoluteLiteral &arg);

        common::bytes::Word value() const;

        size_t size() const override final;

        bool operator==(const Arg &other) const override final;

      private:
        const common::bytes::Word value_;
      };

      class AbsoluteLabel final : public Arg {
      public:
        explicit AbsoluteLabel(std::string_view value);
        explicit AbsoluteLabel(
            const Parser::Program::Instruction::AbsoluteLabel &arg);

        std::string value() const;

        size_t size() const override final;

        bool operator==(const Arg &other) const override final;

      private:
        const std::string value_;
      };

      class ZeroPageIndexed final : public Arg {
      public:
        enum class Index {
          X,
          Y,
        };

        ZeroPageIndexed(Index index, common::bytes::Byte value);
        ZeroPageIndexed(
            const Parser::Program::Instruction::ZeroPageIndexed &arg);

        Index index() const;
        common::bytes::Byte value() const;

        size_t size() const override final;

        bool operator==(const Arg &other) const override final;

      private:
        const Index index_;
        const common::bytes::Byte value_;
      };

      Instruction(common::bytes::Word address,
                  const asm_::Instruction &instruction,
                  std::unique_ptr<Arg> arg);

      Instruction(common::bytes::Word address,
                  const Parser::Program::Instruction &instruction);

      const asm_::Instruction &instruction() const;

      const Arg &arg() const;

      size_t size() const override final;

      bool operator==(const Statement &other) const override final;

    private:
      const asm_::Instruction &instruction_;
      const std::unique_ptr<Arg> arg_;
    };

    // A literal is a set of bytes that are inserted directly into the output.
    class Literal final : public Statement {
    public:
      Literal(common::bytes::Word address,
              std::vector<common::bytes::Byte> values);
      Literal(common::bytes::Word address,
              const Parser::Program::ByteDirective &directive);

      size_t size() const override final;

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
    std::vector<std::unique_ptr<Statement>> statements_;
  };

  Program bind(const Parser::Program &program);
};

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Statement::Type &type);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Arg::Type &type);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::None &arg);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Immediate &arg);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::AbsoluteLiteral &arg);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::AbsoluteLabel &arg);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::ZeroPageIndexed::Index
               &index);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::ZeroPageIndexed &arg);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction::Arg &arg);

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program::Label &label);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Instruction &instruction);

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program::Literal &literal);

std::ostream &
operator<<(std::ostream &os,
           const InstructionBinder::Program::Statement &statement);

std::ostream &operator<<(std::ostream &os,
                         const InstructionBinder::Program &program);

} // namespace irata::assembler
