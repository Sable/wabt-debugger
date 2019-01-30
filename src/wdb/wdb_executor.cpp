#include <wdb/wdb_executor.h>
#include <wabt/src/binary-reader.h>
#include <wabt/src/interp/binary-reader-interp.h>
#include <wabt/src/cast.h>
#include <utility>
#include <iostream>
#include <climits>

namespace wdb {
    WdbExecutor::WdbExecutor(wdb::WdbExecutor::Options options) {
        // Initialize environment
        m_env = new wabt::interp::Environment();
        // Initialize thread
        m_thread = new wabt::interp::Thread(m_env, options.threadOptions);
        // Execute addition pre-setup configuration
        if(options.preSetup) {
            options.preSetup(this);
        }
    }

    wabt::Result WdbExecutor::SetupEnvironment(std::vector<uint8_t> *fileData) {
        // Configure binary reader options
        wabt::ReadBinaryOptions options;
        options.fail_on_custom_section_error = true;
        options.read_debug_names = true;
        options.stop_on_first_error = true;
        // Start reading the binary and setup the environment
        wabt::Errors errors;
        return wabt::ReadBinaryInterp(m_env, fileData->data(), fileData->size(), options, &errors, &m_mainModule);
    }

    wabt::Result WdbExecutor::AppendHostFuncExport(std::string hostName, std::string funcName,
                                         wabt::interp::FuncSignature funcSignature,
                                         std::function<wabt::interp::Result(const wabt::interp::HostFunc *func,
                                                                            const wabt::interp::FuncSignature *sig,
                                                                            const wabt::interp::TypedValues &args,
                                                                            wabt::interp::TypedValues &results)> callback) {
        // Search for module
        wabt::interp::Module* module = m_env->FindRegisteredModule(hostName);
        wabt::interp::HostModule* hostModule = nullptr;
        // If already exists, then use it
        if(module && module->is_host) {
             hostModule = dynamic_cast<wabt::interp::HostModule*>(module);
        } else {
            // Create a new host module
            hostModule = m_env->AppendHostModule(hostName);
        }
        // If casting was successful or new host module
        if(hostModule) {
            hostModule->AppendFuncExport(funcName, funcSignature, std::move(callback));
            return wabt::Result::Ok;
        }
        return wabt::Result::Error;
    }

    wabt::interp::Value WdbExecutor::GetStackAt(int i) {
        return m_thread->ValueAt(i);
    }

    int WdbExecutor::GetStackSize() {
        return m_thread->NumValues();
    }

    char WdbExecutor::GetMemoryAt(int memoryIndex, int elementIndex) {
        return m_env->GetMemory(memoryIndex)->data[elementIndex];
    }

    int WdbExecutor::GetMemorySize(int memoryIndex) {
        return m_env->GetMemory(memoryIndex)->data.size();
    }

    int WdbExecutor::GetMemoriesCount() {
        return m_env->GetMemoryCount();
    }

    wabt::interp::Module* WdbExecutor::GetModuleAt(int index) {
        return m_env->GetModule(index);
    }

    int WdbExecutor::GetModuleSize() {
        return m_env->GetModuleCount();
    }

    std::vector<wabt::interp::Export> WdbExecutor::GetExportedModuleFunctions(wabt::interp::Module *module) {
        std::vector<wabt::interp::Export> result;
        for(wabt::interp::Export e : module->exports) {
            if(e.kind == wabt::ExternalKind::Func) {
                result.emplace_back(e);
            }
        }
        return result;
    }

    wabt::Result WdbExecutor::SearchExportedModuleFunction(wabt::interp::DefinedModule *module, std::string name,
                                                           wabt::interp::Export **pExport) {
        *pExport = nullptr;
        for(auto e : module->exports) {
            if(e.kind == wabt::ExternalKind::Func && e.name == name) {
                *pExport = new wabt::interp::Export(e.name, e.kind, e.index);
                return wabt::Result::Ok;
            }
        }
        return wabt::Result::Error;
    }

    wabt::interp::Func* WdbExecutor::GetFunction(wabt::Index index) {
        return m_env->GetFunc(index);
    }

    wabt::interp::FuncSignature* WdbExecutor::GetFunctionSignature(wabt::Index index) {
        return m_env->GetFuncSignature(index);
    }

    wabt::Result WdbExecutor::SetMainFunction(wabt::interp::Func *function) {
        // Can only have one main function
        if(MainFunctionIsSet() || !CanBeMain(function)) {
            return wabt::Result::Error;
        }
        // Set main function
        m_mainFunction = wabt::cast<wabt::interp::DefinedFunc>(function);
        // Reset thread components
        m_thread->Reset();
        // Set the pc to the function index
        m_thread->set_pc(m_mainFunction->offset);
        return wabt::Result::Ok;
    }

    wabt::Result WdbExecutor::Execute() {
        if(CanRun()) {
            wabt::interp::Result result = wabt::interp::Result::Ok;
            // Keep executing instructions
            while(result == wabt::interp::Result::Ok) {
                result = m_thread->Run(INT_MAX);
            }
            // Main function has returned
            if(result == wabt::interp::Result::Returned) {
                SetMainFunctionReturned();
                return wabt::Result::Ok;
            }
        }
        return wabt::Result::Error;
    }

    wabt::Result WdbExecutor::GetMainFunctionReturnedValues(wabt::interp::TypedValues &values) {
        // Make sure main function is set and has returned
        if(!MainFunctionIsSet() || !MainFunctionHasReturned()) {
            return wabt::Result::Error;
        }
        // Verify that the stack has the return values
        wabt::interp::FuncSignature* sig = m_thread->env()->GetFuncSignature(m_mainFunction->sig_index);
        if(sig->result_types.size() != GetStackSize()) {
            return wabt::Result::Error;
        }
        // Clear values in the provided vector
        values.clear();
        // Copy results from the stack
        for (size_t i = 0; i < GetStackSize(); ++i) {
            values.emplace_back(sig->result_types[i], GetStackAt(i));
        }
        return wabt::Result::Ok;
    }

    bool WdbExecutor::CanBeMain(wabt::interp::Func *func) {
        return !func->is_host && GetFunctionSignature(func->sig_index)->param_types.empty();
    }

    void WdbExecutor::PostOutput(std::string text) {
        if(m_outputStreamHandler) {
            m_outputStreamHandler(std::move(text));
        }
    }

    void WdbExecutor::PostError(std::string text) {
        if(m_errorStreamHandler) {
            m_errorStreamHandler(std::move(text));
        }
    }

    bool WdbExecutor::CanRun() {
        return MainFunctionIsSet() && !MainFunctionHasReturned();
    }
}