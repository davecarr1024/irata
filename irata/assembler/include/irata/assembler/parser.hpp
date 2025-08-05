#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/common/bytes/byte.hpp>
#include <irata/common/bytes/word.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace irata::assembler {

class Parser {
public:
  class Program {
  public:
    class Statement {
    public:
      enum class Type {
        Comment,
        Label,
        Instruction,
      };

      virtual ~Statement() = default;

      virtual Type type() const = 0;

      virtual bool operator==(const Statement &other) const;
      virtual bool operator!=(const Statement &other) const;

    protected:
      Statement() = default;
    };

    template <Statement::Type StatementType>
    class StatementWithType : public virtual Statement {
    public:
      Type type() const override final { return StatementType; }
    };

    class StatementWithValue : public virtual Statement {
    public:
      StatementWithValue(std::string_view value);

      std::string value() const;

      bool operator==(const Statement &other) const override;

    private:
      std::string value_;
    };

    class Comment final : public StatementWithType<Statement::Type::Comment>,
                          public StatementWithValue {
    public:
      explicit Comment(std::string_view value);
    };

    class Label final : public StatementWithType<Statement::Type::Label>,
                        public StatementWithValue {
    public:
      explicit Label(std::string_view value);
    };

    class Instruction final
        : public StatementWithType<Statement::Type::Instruction> {
    public:
      class Arg {
      public:
        enum class Type {
          None,
          Immediate,
          AbsoluteLiteral,
          AbsoluteLabel,
        };

        virtual ~Arg() = default;

        virtual asm_::AddressingMode addressing_mode() const = 0;

        virtual Type type() const = 0;

        virtual bool operator==(const Arg &other) const;
        bool operator!=(const Arg &other) const;

      protected:
        Arg() = default;
      };

      template <Arg::Type ArgType> class ArgWithType : public virtual Arg {
      public:
        Type type() const override final { return ArgType; }
      };

      template <asm_::AddressingMode AddressingMode>
      class ArgWithAddressingMode : public virtual Arg {
      public:
        asm_::AddressingMode addressing_mode() const override final {
          return AddressingMode;
        }
      };

      class None final
          : public ArgWithType<Arg::Type::None>,
            public ArgWithAddressingMode<asm_::AddressingMode::NONE> {
      public:
        None() = default;
      };

      template <typename T> class ArgWithValue : public virtual Arg {
      public:
        explicit ArgWithValue(T value) : value_(value) {}

        T value() const { return value_; }

        bool operator==(const Arg &other) const override {
          return Arg::operator==(other) &&
                 value_ == dynamic_cast<const ArgWithValue<T> &>(other).value_;
        }

      private:
        T value_;
      };

      class Immediate final
          : public ArgWithType<Arg::Type::Immediate>,
            public ArgWithAddressingMode<asm_::AddressingMode::IMMEDIATE>,
            public ArgWithValue<common::bytes::Byte> {
      public:
        explicit Immediate(common::bytes::Byte value);

        bool operator==(const Arg &other) const override final;
      };

      class AbsoluteLiteral final
          : public ArgWithType<Arg::Type::AbsoluteLiteral>,
            public ArgWithAddressingMode<asm_::AddressingMode::ABSOLUTE>,
            public ArgWithValue<common::bytes::Word> {
      public:
        explicit AbsoluteLiteral(common::bytes::Word value);

        bool operator==(const Arg &other) const override final;
      };

      class AbsoluteLabel
          : public ArgWithType<Arg::Type::AbsoluteLabel>,
            public ArgWithAddressingMode<asm_::AddressingMode::ABSOLUTE>,
            public ArgWithValue<std::string> {
      public:
        explicit AbsoluteLabel(std::string_view value);

        bool operator==(const Arg &other) const override final;
      };

      Instruction(std::string_view instruction, std::unique_ptr<Arg> arg);

      bool operator==(const Statement &other) const override final;

      std::string instruction() const;

      const Arg &arg() const;

    private:
      std::string instruction_;
      std::unique_ptr<Arg> arg_;
    };

    explicit Program(std::vector<std::unique_ptr<Statement>> statements);

    const std::vector<std::unique_ptr<Statement>> &statements() const;

    bool operator==(const Program &other) const;
    bool operator!=(const Program &other) const;

  private:
    std::vector<std::unique_ptr<Statement>> statements_;
  };

  Program parse(std::string_view input);
};

} // namespace irata::assembler
