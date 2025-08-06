#include <irata/assembler/instruction_binder.hpp>
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
        Instruction,
      };

      virtual ~Statement() = default;

      static std::unique_ptr<Statement>
      bind(const InstructionBinder::Program::Statement &statement,
           const BindContext &context);

      Type type() const;

      common::bytes::Word address() const;

      virtual bool operator==(const Statement &other) const;
      virtual bool operator!=(const Statement &other) const;

    protected:
      Statement(Type type, common::bytes::Word address);

    private:
      const Type type_;
      const common::bytes::Word address_;
    };

    class Instruction final : public Statement {
    public:
      class Arg {
      public:
        enum class Type {
          None,
          Immediate,
          Absolute,
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

      Instruction(common::bytes::Word address,
                  const asm_::Instruction &instruction,
                  std::unique_ptr<Arg> arg);
      Instruction(const InstructionBinder::Program::Instruction &instruction,
                  const BindContext &context);

      const asm_::Instruction &instruction() const;

      const Arg &arg() const;

      bool operator==(const Statement &other) const override final;

    private:
      const asm_::Instruction &instruction_;
      const std::unique_ptr<Arg> arg_;
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

} // namespace irata::assembler
