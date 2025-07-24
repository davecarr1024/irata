// #include <algorithm>
// #include <functional>
// #include <irata/sim/hdl/hdl.hpp>
// #include <irata/sim/microcode/compiler/compiler.hpp>
// #include <irata/sim/microcode/dsl/instruction.hpp>
// #include <irata/sim/microcode/dsl/instruction_set.hpp>
// #include <irata/sim/microcode/dsl/step.hpp>
// #include <sstream>
// #include <stdexcept>
// #include <string>
// #include <vector>

// namespace irata::sim::microcode::compiler {

// namespace {

// struct ValidationError {
//   const std::string message;
// };

// using ValidationErrors = std::vector<ValidationError>;

// ValidationErrors validate_and_append(ValidationErrors errors,
//                                      std::function<ValidationErrors()> f) {
//   auto new_errors = f();
//   for (auto &error : new_errors) {
//     errors.push_back(std::move(error));
//   }
//   return errors;
// }

// struct Step {
//   std::set<const hdl::ControlDecl *> controls;
//   std::set<const hdl::WriteControlDecl *> write_controls;
//   std::set<const hdl::ReadControlDecl *> read_controls;

//   static Step from_dsl(const dsl::Step &step) {
//     return Step{
//         .controls = step.controls(),
//         .write_controls = step.write_controls(),
//         .read_controls = step.read_controls(),
//     };
//   }

//   Step merge(const Step &other) const {
//     Step step = *this;
//     step.controls.insert(other.controls.begin(), other.controls.end());
//     step.write_controls.insert(other.write_controls.begin(),
//                                other.write_controls.end());
//     step.read_controls.insert(other.read_controls.begin(),
//                               other.read_controls.end());
//     return step;
//   }

//   hdl::TickPhase max_phase() const {
//     hdl::TickPhase phase = hdl::TickPhase::Control;
//     for (const auto &control : controls) {
//       phase = std::max(phase, control->phase());
//     }
//     return phase;
//   }

//   hdl::TickPhase min_phase() const {
//     hdl::TickPhase phase = hdl::TickPhase::Clear;
//     for (const auto &control : controls) {
//       phase = std::min(phase, control->phase());
//     }
//     return phase;
//   }

//   bool can_merge(const Step &other) const {
//     return max_phase() <= other.min_phase();
//   }

//   struct BusControls {
//     std::set<const hdl::WriteControlDecl *> write_controls;
//     std::set<const hdl::ReadControlDecl *> read_controls;
//   };

//   std::map<const hdl::BusDecl *, BusControls> group_controls_by_bus() const {
//     std::map<const hdl::BusDecl *, BusControls> result;
//     for (const auto &control : write_controls) {
//       result[&control->bus()].write_controls.insert(control);
//     }
//     for (const auto &control : read_controls) {
//       result[&control->bus()].read_controls.insert(control);
//     }
//     return result;
//   }

//   std::string to_string() const {
//     std::ostringstream os;
//     os << "Step(";
//     os << "controls={";
//     for (const auto &control : controls) {
//       os << *control << ", ";
//     }
//     os << "})";
//     return os.str();
//   }

//   ValidationErrors validate_bus_conflicts() const {
//     ValidationErrors errors;
//     const auto controls_by_bus = group_controls_by_bus();
//     for (const auto &[bus, controls] : controls_by_bus) {
//       if (controls.write_controls.size() == 0) {
//         std::ostringstream os;
//         os << "Step " << to_string() << " has no write controls for bus "
//            << *bus;
//         errors.push_back(ValidationError{.message = os.str()});
//       }
//       if (controls.write_controls.size() > 1) {
//         std::ostringstream os;
//         os << "Step " << to_string() << " has multiple write controls for bus
//         "
//            << *bus << ": ";
//         for (const auto &control : controls.write_controls) {
//           os << *control << ", ";
//         }
//         errors.push_back(ValidationError{.message = os.str()});
//       }
//       if (controls.read_controls.size() == 0) {
//         std::ostringstream os;
//         os << "Step " << to_string() << " has no read controls for bus "
//            << *bus;
//         errors.push_back(ValidationError{.message = os.str()});
//       }
//     }
//     return errors;
//   }

//   ValidationErrors validate() const {
//     ValidationErrors errors;
//     errors = validate_and_append(std::move(errors),
//                                  [&] { return validate_bus_conflicts(); });
//     return errors;
//   }

//   table::Step compile() const {
//     std::set<std::string> controls;
//     for (const auto &control : this->controls) {
//       controls.insert(control->path());
//     }
//     return table::Step{.controls = controls};
//   }
// };

// std::ostream &operator<<(std::ostream &os, const Step &step) {
//   os << step.to_string();
//   return os;
// }

// struct Instruction {
//   const asm_::Instruction &descriptor;
//   std::vector<Step> steps;
//   std::map<const hdl::StatusDecl *, bool> statuses;

//   static Instruction from_dsl(const dsl::Instruction &instruction) {
//     Instruction result{
//         .descriptor = instruction.instruction(),
//         .statuses = instruction.statuses(),
//     };
//     for (const auto &step : instruction.steps()) {
//       result.steps.push_back(Step::from_dsl(*step));
//     }
//     return result;
//   }

//   ValidationErrors validate() const {
//     ValidationErrors errors;
//     for (const auto &step : steps) {
//       errors = validate_and_append(std::move(errors),
//                                    [&] { return step.validate(); });
//     }
//     return errors;
//   }

//   table::InstructionVariant compile() const {
//     std::vector<table::Step> steps;
//     for (const auto &step : this->steps) {
//       steps.push_back(step.compile());
//     }
//     std::map<std::string, bool> statuses;
//     for (const auto &[status, value] : this->statuses) {
//       statuses[status->path()] = value;
//     }
//     return table::InstructionVariant{.steps = steps, .statuses = statuses};
//   }

//   Instruction optimize() const {
//     Instruction result{.descriptor = descriptor, .statuses = statuses};
//     if (steps.empty()) {
//       return result;
//     }
//     std::vector<Step> steps = this->steps;
//     Step step = steps.front();
//     steps.erase(steps.begin());
//     while (!steps.empty()) {
//       const Step next_step = steps.front();
//       steps.erase(steps.begin());
//       if (step.can_merge(next_step)) {
//         step = step.merge(next_step);
//       } else {
//         result.steps.push_back(step);
//         step = next_step;
//       }
//     }
//     result.steps.push_back(step);
//     return result;
//   }
// };

// struct InstructionSet {
//   std::vector<Instruction> instructions;

//   static InstructionSet from_dsl(const dsl::InstructionSet &instruction_set)
//   {
//     InstructionSet result;
//     for (const auto &instruction : instruction_set.instructions()) {
//       result.instructions.push_back(Instruction::from_dsl(*instruction));
//     }
//     return result;
//   }

//   ValidationErrors validate_complementary_variants() const {
//     ValidationErrors errors;
//     const auto instructions_by_descriptor =
//     group_instructions_by_descriptor();

//     for (const auto &[descriptor, instructions] : instructions_by_descriptor)
//     {
//       for (const auto &instruction : instructions) {
//         std::map<const hdl::StatusDecl *, bool> complementary_status;
//         for (const auto &[status, value] : instruction->statuses) {
//           complementary_status[status] = !value;
//         }

//         // Search for a matching complementary variant
//         bool found = false;
//         for (const auto &other_instruction : instructions) {
//           if (&other_instruction == &instruction) {
//             continue;
//           }
//           if (other_instruction->statuses == complementary_status) {
//             found = true;
//             break;
//           }
//         }

//         if (!found && !complementary_status.empty()) {
//           std::ostringstream os;
//           os << "Instruction " << descriptor->name
//              << " is missing complementary variant for statuses ";
//           for (const auto &[status, value] : instruction->statuses) {
//             os << status->name() << "=" << value << ", ";
//           }
//           errors.push_back(ValidationError{.message = os.str()});
//         }
//       }
//     }

//     return errors;
//   }

//   ValidationErrors validate() const {
//     ValidationErrors errors;
//     for (const auto &instruction : instructions) {
//       errors = validate_and_append(std::move(errors),
//                                    [&] { return instruction.validate(); });
//     }
//     errors = validate_and_append(
//         std::move(errors), [&] { return validate_complementary_variants();
//         });
//     return errors;
//   }

//   std::map<const asm_::Instruction *, std::vector<const Instruction *>>
//   group_instructions_by_descriptor() const {
//     std::map<const asm_::Instruction *, std::vector<const Instruction *>>
//         result;
//     for (const auto &instruction : instructions) {
//       result[&instruction.descriptor].push_back(&instruction);
//     }
//     return result;
//   }

//   table::Table compile() const {
//     const auto instructions_by_descriptor =
//     group_instructions_by_descriptor(); std::vector<table::Instruction>
//     table_instructions; for (const auto &[descriptor, instructions] :
//     instructions_by_descriptor) {
//       std::vector<table::InstructionVariant> variants;
//       for (const auto &instruction : instructions) {
//         variants.push_back(instruction->compile());
//       }
//       table_instructions.push_back(table::Instruction{
//           .descriptor = *descriptor,
//           .variants = std::move(variants),
//       });
//     }
//     return table::Table{.instructions = std::move(table_instructions)};
//   }

//   InstructionSet optimize() const {
//     InstructionSet result;
//     for (const auto &instruction : instructions) {
//       result.instructions.push_back(instruction.optimize());
//     }
//     return result;
//   }
// };

// void validate(const InstructionSet &instruction_set) {
//   const auto errors = instruction_set.validate();
//   if (!errors.empty()) {
//     std::ostringstream os;
//     os << "Validation errors:\n";
//     for (const auto &error : errors) {
//       os << "  " << error.message << "\n";
//     }
//     throw std::invalid_argument(os.str());
//   }
// }

// } // namespace

// void validate(const dsl::InstructionSet &instruction_set) {
//   const auto instruction_set_ = InstructionSet::from_dsl(instruction_set);
//   validate(instruction_set_);
// }

// table::Table compile(const dsl::InstructionSet &instruction_set) {
//   auto instruction_set_ = InstructionSet::from_dsl(instruction_set);
//   validate(instruction_set_);
//   instruction_set_ = instruction_set_.optimize();
//   validate(instruction_set_);
//   return instruction_set_.compile();
// }

// } // namespace irata::sim::microcode::compiler
