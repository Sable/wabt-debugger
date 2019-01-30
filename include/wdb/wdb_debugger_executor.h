#ifndef WDB_WDB_DEBUGGER_EXECUTOR_H
#define WDB_WDB_DEBUGGER_EXECUTOR_H

#include <wdb/wdb_executor.h>
#include <set>

namespace wdb {
    class WdbDebuggerExecutor : public WdbExecutor {
    public:
        struct Instruction {
            wabt::interp::IstreamOffset istream_start;
            std::string str;
        };

        /**
         * Create a debugger executor
         * @param options
         */
        WdbDebuggerExecutor(WdbExecutor::Options options);

        /**
         * Execute next instruction
         * @return result
         */
        wabt::Result ExecuteNextInstruction();

        /**
         * Continue executing instructions till hitting return or break point
         * @return result
         */
        wabt::Result Execute();

        /**
         * Add breakpoint
         * @param offset
         */
        void AddBreakpoint(wabt::interp::IstreamOffset offset);

        /**
         * Remove breakpoint
         * @param offset
         */
        void RemoveBreakpoint(wabt::interp::IstreamOffset offset);

        /**
         * Get break points
         * @return vector of breakpoints
         */
        std::set<wabt::interp::IstreamOffset> GetBreakpoints() const { return m_breakPc; }

        /**
         * Get disassembled module
         * @param module
         *
         * Note: This code is a modified version of the method Environment::Disassemble()
         */
        std::vector<Instruction> DisassembleModule(wabt::interp::DefinedModule* module);

        /**
         * Get relative pc offset
         * @return pc offset
         */
        wabt::interp::IstreamOffset GetPcOffset();
    private:
        std::set<wabt::interp::IstreamOffset> m_breakPc;
    };
}

#endif