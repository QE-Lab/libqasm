/** \file
 * Implementation for \ref include/cqasm-instruction.hpp "cqasm-instruction.hpp".
 */

#include "cqasm-instruction.hpp"
#include "cqasm-utils.hpp"
#include "cqasm-semantic.hpp"

namespace cqasm {
namespace instruction {

/**
 * Creates a new instruction. param_types is a shorthand type specification
 * string as parsed by cqasm::types::from_spec(). If you need more control,
 * you can also manipulate param_types directly.
 *
 * allow_conditional specifies whether the instruction can be made
 * conditional with c- notation. allow_parallel specifies whether it may
 * appear bundled with other instructions. allow_reused_qubits specifies
 * whether it is legal for the instruction to use a qubit more than once in
 * its parameter list. allow_different_index_sizes specifies whether it's legal
 * to have different "index sizes" for different parameters, for instance
 * q[1,2] in one parameter and q[1,2,3,4,5] in another.
 */
Instruction::Instruction(
    const std::string &name,
    const std::string &param_types,
    bool allow_conditional,
    bool allow_parallel,
    bool allow_reused_qubits,
    bool allow_different_index_sizes
) :
    name(name),
    param_types(types::from_spec(param_types)),
    allow_conditional(allow_conditional),
    allow_parallel(allow_parallel),
    allow_reused_qubits(allow_reused_qubits),
    allow_different_index_sizes(allow_different_index_sizes)
{}

/**
 * Equality operator.
 */
bool Instruction::operator==(const Instruction& rhs) const {
    return utils::case_insensitive_equals(name, rhs.name)
            && param_types == rhs.param_types
            && allow_conditional == rhs.allow_conditional
            && allow_parallel == rhs.allow_parallel
            && allow_reused_qubits == rhs.allow_reused_qubits;
}

} // namespace instruction

namespace primitives {

template <>
void serialize<instruction::InstructionRef>(
    const instruction::InstructionRef &obj,
    ::tree::cbor::MapWriter &map
) {
    // Use an empty map to signal an empty instruction reference.
    if (obj.empty()) {
        return;
    }

    // Serialize the fields that are basically primitives.
    map.append_string("n", obj->name);
    map.append_bool("ac", obj->allow_conditional);
    map.append_bool("ap", obj->allow_parallel);
    map.append_bool("ar", obj->allow_reused_qubits);
    map.append_bool("ad", obj->allow_different_index_sizes);

    // Serialize the parameter types, which behave like a subtree.
    ::tree::base::PointerMap types_id_map{};
    obj->param_types.find_reachable(types_id_map);
    auto types = map.append_map("t");
    obj->param_types.serialize(types, types_id_map);

}

template <>
instruction::InstructionRef deserialize<instruction::InstructionRef>(
    const ::tree::cbor::MapReader &map
) {
    // Empty map signals empty reference.
    if (map.empty()) {
        return instruction::InstructionRef{};
    }

    // Restore the fields that are basically primitives.
    auto obj = tree::make<instruction::Instruction>(map.at("n").as_string());
    obj->allow_conditional = map.at("ac").as_bool();
    obj->allow_parallel = map.at("ap").as_bool();
    obj->allow_reused_qubits = map.at("ar").as_bool();
    obj->allow_different_index_sizes = map.at("ad").as_bool();

    // Restore the types subtree.
    ::tree::base::IdentifierMap types_id_map{};
    obj->param_types = tree::Any<types::TypeBase>(map.at("t").as_map(), types_id_map);
    types_id_map.restore_links();
    obj->param_types.check_well_formed();

    return std::move(obj);
}

} // namespace primitives
} // namespace cqasm

/**
 * Stream << overload for instructions.
 */
std::ostream& operator<<(std::ostream& os, const ::cqasm::instruction::Instruction& insn) {
    os << insn.name << insn.param_types;
    return os;
}

/**
 * Stream << overload for instruction references.
 */
std::ostream& operator<<(std::ostream& os, const ::cqasm::instruction::InstructionRef& insn) {
    if (insn.empty()) {
        os << "unresolved";
    } else {
        os << *insn;
    }
    return os;
}
