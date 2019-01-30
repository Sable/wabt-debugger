#ifndef WDB_WDB_EXECUTOR_H
#define WDB_WDB_EXECUTOR_H

#include <wabt/src/result.h>
#include <wabt/src/stream.h>
#include <wabt/src/feature.h>
#include <wabt/src/interp/interp.h>
#include <wabt/src/error-formatter.h>
#include <sstream>

namespace wdb {
    class WdbExecutor {
    public:
        struct Options {
            wabt::interp::Thread::Options threadOptions;
            std::function<void(std::string text)> outputStreamHandler;
            std::function<void(std::string text)> errorStreamHandler;
            std::function<void(wdb::WdbExecutor*)> preSetup;
        };
        /**
         * Construct an executor
         * @param options
         */
        WdbExecutor(WdbExecutor::Options options);

        /**
         * Get program stack
         * @param index
         */
        wabt::interp::Value GetStackAt(int index);

        /**
         * Get stack size
         * @return stack size
         */
        int GetStackSize();

        /**
         * Get memory at
         * @param memoryIndex
         * @param elementIndex
         * @returna char
         */
        char GetMemoryAt(int memoryIndex, int elementIndex);

        /**
         * Get size of memory at index
         * @param memoryIndex
         * @return size of memory
         */
        int GetMemorySize(int memoryIndex);

        /**
         * Get the number of memories
         * @return memories count
         */
        int GetMemoriesCount();

        /**
         * Get module at index
         * @param index
         * @return module
         */
        wabt::interp::Module* GetModuleAt(int index);

        /**
         * Get module size
         * @return module size
         */
        int GetModuleSize();

        /**
         * Setup the program environment
         * @return result
         */
        wabt::Result SetupEnvironment(std::vector<uint8_t> *fileData);

        /**
         * Append host function
         * @param hostName
         * @param funcName
         * @param funcSignature
         * @param callback
         */
        wabt::Result AppendHostFuncExport(std::string hostName, std::string funcName, wabt::interp::FuncSignature funcSignature,
                                std::function<wabt::interp::Result (const wabt::interp::HostFunc* func,
                                                                    const wabt::interp::FuncSignature* sig,
                                                                    const wabt::interp::TypedValues& args,
                                                                    wabt::interp::TypedValues& results)> callback);

        /**
         * Get the main module
         * @return module
         */
        wabt::interp::DefinedModule* GetMainModule() const { return m_mainModule; }

        /**
         * Get exported functions in a module
         * @param module
         * @return vector of exported functions
         */
        std::vector<wabt::interp::Export> GetExportedModuleFunctions(wabt::interp::Module* module);

        /**
         * Search for exported function in a module
         * @param module
         * @param name
         * @param pExport
         * @return result
         */
        wabt::Result SearchExportedModuleFunction(wabt::interp::DefinedModule* module, std::string name,
                                                          wabt::interp::Export **pExport);

        /**
         * Get function by index
         * @param index
         * @return function
         */
        wabt::interp::Func* GetFunction(wabt::Index index);

        /**
         * Get function signature
         * @param index
         * @return function signature
         */
        wabt::interp::FuncSignature* GetFunctionSignature(wabt::Index index);

        /**
         * Set the program counter at the function index
         * @param function
         */
        wabt::Result SetMainFunction(wabt::interp::Func* function);

        /**
         * Continue executing instructions till hitting return
         * @return result
         */
        virtual wabt::Result Execute();

        /**
         * Get main function returned values
         * @return Errors
         */
        wabt::Result GetMainFunctionReturnedValues(wabt::interp::TypedValues &values);

        /**
         * Check if main function has returned
         * @return true if it did
         */
        bool MainFunctionHasReturned() const { return m_mainReturned; }

        /**
         * Check if a function can be executed
         * @param func
         * @return true if can be main
         */
        bool CanBeMain(wabt::interp::Func* func);

        /**
         * Set output stream handler
         * @param handler
         */
        void SetOutputStreamHandler(std::function<void(std::string)> handler) { m_outputStreamHandler = handler; }

        /**
         * Set error stream handler
         * @param handler
         */
        void SetErrorStreamHandler(std::function<void(std::string)> handler) { m_errorStreamHandler = handler; }

        /**
         * Post output text
         * @param text
         */
        void PostOutput(std::string text);

        /**
         * Post error text
         * @param text
         */
        void PostError(std::string text);

        /**
         * Check if main function is set
         * @return true if is set
         */
        bool MainFunctionIsSet() const { return m_mainFunction; }
    protected:
        wabt::interp::Thread* m_thread = nullptr;
        wabt::interp::Environment* m_env = nullptr;

        /**
         * Check if can run instruction
         * @return true if can run
         */
        bool CanRun();

        /**
         * Mark main has returned
         */
        void SetMainFunctionReturned() { m_mainReturned = true; };
    private:
        wabt::interp::DefinedModule* m_mainModule = nullptr;
        wabt::interp::DefinedFunc* m_mainFunction = nullptr;
        bool m_mainReturned = false;
        std::function<void(std::string)> m_outputStreamHandler;
        std::function<void(std::string)> m_errorStreamHandler;
    };
}

#endif
