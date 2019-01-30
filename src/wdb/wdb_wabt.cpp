#include <wdb/wdb_wabt.h>
#include <wabt/src/binary-reader.h>
#include <wabt/src/interp/binary-reader-interp.h>
#include <wabt/src/cast.h>
#include <wabt/src/interp/interp-internal.h>

namespace wdb {
    wabt::Result WdbWabt::LoadModuleFile(std::string fileName) {
        // Set file name
        m_fileName = fileName;
        // Read file data
        return wabt::ReadFile(fileName, &m_fileData);
    }

    wdb::WdbExecutor* WdbWabt::CreateWdbExecutor(wdb::WdbExecutor::Options options) {
        auto executor = new WdbExecutor(options);
        if(wabt::Succeeded(ConfigureExecutor(executor, options))) {
            return executor;
        }
        delete executor;
        return nullptr;
    }

    wdb::WdbDebuggerExecutor* WdbWabt::CreateWdbDebuggerExecutor(wdb::WdbExecutor::Options options) {
        auto executor = new WdbDebuggerExecutor(options);
        if(wabt::Succeeded(ConfigureExecutor(executor, options))) {
            return executor;
        }
        delete executor;
        return nullptr;
    }

    wdb::WdbProfilerExecutor* WdbWabt::CreateWdbProfilerExecutor(wdb::WdbExecutor::Options options) {
        auto executor = new WdbProfilerExecutor(options);
        if(wabt::Succeeded(ConfigureExecutor(executor, options))) {
            return executor;
        }
        delete executor;
        return nullptr;
    }

    wdb::WdbCodeGen* WdbWabt::CreateCodeGenerator() {
        auto codeGenerator = new WdbCodeGen();
        if(wabt::Succeeded(codeGenerator->SetupCode(&m_fileData))) {
            return codeGenerator;
        }
        delete codeGenerator;
        return nullptr;
    }

    wabt::Result WdbWabt::ConfigureExecutor(wdb::WdbExecutor *executor, wdb::WdbExecutor::Options options) {
        if(!wabt::Succeeded(executor->SetupEnvironment(&m_fileData))) {
            return wabt::Result::Error;
        }
        executor->SetOutputStreamHandler(options.outputStreamHandler);
        executor->SetErrorStreamHandler(options.errorStreamHandler);
        return wabt::Result::Ok;
    }
}