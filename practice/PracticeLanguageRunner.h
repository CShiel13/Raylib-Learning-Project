#pragma once
#include <string>
#include "PracticeTypes.h"

class PracticeLanguageRunner {
public:
    virtual ~PracticeLanguageRunner() = default;
    virtual const std::string& LanguageName() const = 0;
    virtual const std::string& StarterCode() const = 0;
    virtual const std::string& Instructions() const = 0;
    virtual bool PlatformSupported() const = 0;
    virtual bool Available() const = 0;
    virtual std::string SetupMessage() const = 0;
    virtual PracticeRunResult Run(const std::string& source, int side, PracticeRunMode mode = PracticeRunMode::GridDrawing) const = 0;
};
