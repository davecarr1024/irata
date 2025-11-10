#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/assembler/index.hpp>
#include <irata/assembler/source_location.hpp>
#include <irata/common/bytes/byte.hpp>
#include <irata/common/bytes/word.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace irata::assembler {

// Parses a program into an intermediate representation.
class Parser final {
public:
  // An intermediate representation of a program.
  class Program {
  public:
    // A single statement in a program.
    class Statement {
    public:
      enum class Type {
        Comment,
        Label,
        Instruction,
        ByteDirective,
      };

      virtual ~Statement() = default;

      Type type() const;
      const SourceLocation &source_location() const;

      virtual bool operator==(const Statement &other) const;
      virtual bool operator!=(const Statement &other) const;

      static void parse(std::vector<std::unique_ptr<Statement>> &statements,
                        std::string_view line, const SourceLocation &location);

    protected:
      Statement(Type type, SourceLocation location);

    private:
      const Type type_;
      const SourceLocation source_location_;
    };

    class StatementWithValue : public Statement {
    public:
      std::string value() const;

      bool operator==(const Statement &other) const override;

    protected:
      StatementWithValue(Type type, std::string_view value, SourceLocation location);

    private:
      std::string value_;
    };

    // A comment in a program, of the form ";  the comment", alone on a line or
    // at the end of a line.
    class Comment final : public StatementWithValue {
    public:
      Comment(std::string_view value, SourceLocation location);
    };

    // A label in a program, of the form "label:", alone on a line or at the
    // start of a line.
    class Label final : public StatementWithValue {
    public:
      Label(std::string_view value, SourceLocation location);
    };

    // An instruction in a program, of the form "instruction arg".
    class Instruction final : public Statement {
    public:
      // An argument to an instruction.
      class Arg {
      public:
        enum class Type {
          None,
          Immediate,
          AbsoluteLiteral,
          AbsoluteLabel,
          ZeroPageIndexed,
          AbsoluteIndexed,
        };

        virtual ~Arg() = default;

        static std::unique_ptr<Arg> parse(std::string_view arg);

        Type type() const;

        asm_::AddressingMode addressing_mode() const;

        virtual bool operator==(const Arg &other) const;
        bool operator!=(const Arg &other) const;

      protected:
        Arg(Type type, asm_::AddressingMode addressing_mode);

      private:
        const Type type_;
        const asm_::AddressingMode addressing_mode_;
      };

      // No argument to an instruction. Of the form "instruction".
      class None final : public Arg {
      public:
        None();
      };

      template <typename T> class ArgWithValue : public Arg {
      public:
        ArgWithValue(Type type, asm_::AddressingMode addressing_mode, T value)
            : Arg(type, addressing_mode), value_(value) {}

        T value() const { return value_; }

        bool operator==(const Arg &other) const override {
          return Arg::operator==(other) &&
                 value_ == dynamic_cast<const ArgWithValue<T> &>(other).value_;
        }

      private:
        const T value_;
      };

      // An immediate argument to an instruction. Of the form "instruction
      // #value". Note that value can be #decimal or #$hex.
      class Immediate final : public ArgWithValue<common::bytes::Byte> {
      public:
        explicit Immediate(common::bytes::Byte value);
      };

      // An absolute literal argument to an instruction. Of the form
      // "instruction value". Note that value can be decimal or $hex.
      class AbsoluteLiteral final : public ArgWithValue<common::bytes::Word> {
      public:
        explicit AbsoluteLiteral(common::bytes::Word value);
      };

      // An absolute label argument to an instruction. Of the form
      // "instruction label".
      class AbsoluteLabel : public ArgWithValue<std::string> {
      public:
        explicit AbsoluteLabel(std::string_view value);
      };

      class ZeroPageIndexed final : public Arg {
      public:
        ZeroPageIndexed(Index index, common::bytes::Byte value);

        Index index() const;
        common::bytes::Byte value() const;

        bool operator==(const Arg &other) const override final;

        static std::unique_ptr<ZeroPageIndexed> parse(std::string_view arg);

      private:
        const Index index_;
        const common::bytes::Byte value_;
      };

      class AbsoluteIndexed final : public Arg {
      public:
        AbsoluteIndexed(Index index, common::bytes::Word value);

        Index index() const;
        common::bytes::Word value() const;

        bool operator==(const Arg &other) const override final;

        static std::unique_ptr<AbsoluteIndexed> parse(std::string_view arg);

      private:
        const Index index_;
        const common::bytes::Word value_;
      };

      Instruction(std::string_view instruction, std::unique_ptr<Arg> arg, SourceLocation location);

      static std::unique_ptr<Instruction> parse(std::string_view line, const SourceLocation &location);

      bool operator==(const Statement &other) const override final;

      std::string instruction() const;

      const Arg &arg() const;

    private:
      const std::string instruction_;
      const std::unique_ptr<Arg> arg_;
    };

    class ByteDirective final : public Statement {
    public:
      ByteDirective(common::bytes::Byte value, SourceLocation location);

      common::bytes::Byte value() const;

      bool operator==(const Statement &other) const override final;

    private:
      const common::bytes::Byte value_;
    };

    explicit Program(std::vector<std::unique_ptr<Statement>> statements);

    static Program parse(std::string_view input, std::string_view filename = "<stdin>");

    const std::vector<std::unique_ptr<Statement>> &statements() const;

    bool operator==(const Program &other) const;
    bool operator!=(const Program &other) const;

  private:
    std::vector<std::unique_ptr<Statement>> statements_;
  };

  // Parses the given input into a program.
  Program parse(std::string_view input, std::string_view filename = "<stdin>");
};

std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Statement::Type &type);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Arg::Type &type);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::None &arg);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Immediate &arg);
std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::AbsoluteLiteral &arg);
std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::AbsoluteLabel &arg);
std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::ZeroPageIndexed &arg);
std::ostream &
operator<<(std::ostream &os,
           const Parser::Program::Instruction::AbsoluteIndexed &arg);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction::Arg &arg);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Comment &comment);
std::ostream &operator<<(std::ostream &os, const Parser::Program::Label &label);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Instruction &instruction);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::ByteDirective &statement);
std::ostream &operator<<(std::ostream &os,
                         const Parser::Program::Statement &statement);
std::ostream &operator<<(std::ostream &os, const Parser::Program &program);

} // namespace irata::assembler
