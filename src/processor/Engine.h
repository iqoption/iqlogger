//
// Engine.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>
#include <libplatform/libplatform.h>
#include <v8.h>

#include "core/Singleton.h"
#include "core/Log.h"

namespace iqlogger::processor {

    class Engine : public Singleton<Engine>
    {
        friend class Singleton<Engine>;
        Engine();

        std::unique_ptr<v8::Platform> m_platform;

    public:

        virtual ~Engine();
        v8::Platform* getPlatformPtr() const;
        bool checkScript(const std::string& script_source) const;
        void processException(const std::string& msg, v8::Isolate* isolate, v8::TryCatch* try_catch) const;
    };

    using EnginePtr = std::shared_ptr<Engine>;
}


