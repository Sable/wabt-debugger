#ifndef WDB_WDB_WABT_H
#define WDB_WDB_WABT_H

#include <wdb/wdb_profiler_executor.h>
#include <wdb/wdb_debugger_executor.h>
#include <wdb/wdb_code_gen.h>

namespace wdb {
    class WdbWabt {
    public:
        /**
         * Load module file
         * @param fileName
         * @return result
         */
        wabt::Result LoadModuleFile(std::string fileName);

        /**
         * Create an isolated executor with a new environment and thread
         * @param options
         * @return Wdb Executor
         */
        wdb::WdbExecutor* CreateWdbExecutor(wdb::WdbExecutor::Options options);

        /**
         * Create an isolated debugger executor with a new environment and thread
         * @param options
         * @return Wdb Debugger Executor
         */
        wdb::WdbDebuggerExecutor* CreateWdbDebuggerExecutor(wdb::WdbExecutor::Options options);

        /**
         * Create an isolated profiler executor with a new environment and thread
         * @param options
         * @return Wdb Profiler Executor
         */
        wdb::WdbProfilerExecutor* CreateWdbProfilerExecutor(wdb::WdbExecutor::Options options);

        /**
         * Create a code generator instance
         * @return code generator
         */
        wdb::WdbCodeGen* CreateCodeGenerator();
    private:
        std::string m_fileName;
        std::vector<uint8_t> m_fileData;

        /**
         * Configure executor
         * @param executor
         * @param options
         * @return result
         */
        wabt::Result ConfigureExecutor(WdbExecutor* executor, WdbExecutor::Options options);
    };
}

#endif
