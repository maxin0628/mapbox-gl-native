#include <android_native_app_glue.h>
#include <mbgl/benchmark.hpp>
#include <mbgl/util/logging.hpp>

#include <android/asset_manager.h>
#include <android/log.h>

#include "jni.hpp"

#include <cstdio>
#include <string>

namespace mbgl {

namespace {

int severityToPriority(EventSeverity severity) {
    switch (severity) {
        case EventSeverity::Debug:
            return ANDROID_LOG_DEBUG;
        case EventSeverity::Info:
            return ANDROID_LOG_INFO;
        case EventSeverity::Warning:
            return ANDROID_LOG_WARN;
        case EventSeverity::Error:
            return ANDROID_LOG_ERROR;
        default:
            return ANDROID_LOG_VERBOSE;
    }
}

} // namespace

void Log::platformRecord(EventSeverity severity, const std::string& msg) {
    __android_log_print(severityToPriority(severity), "mbgl", "%s", msg.c_str());
}

} // namespace mbgl

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, NULL);

    std::vector<std::string> arguments = {"mbgl-benchmark-runner",
                                          "--benchmark_repetitions=3",
                                          "--benchmark_format=json",
                                          "--benchmark_out=/sdcard/performance/results.json"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back((char*)arg.data());
    }
    argv.push_back(nullptr);
    mbgl::Log::Info(mbgl::Event::General, "Start running BenchmarkRunner");
    int status = mbgl::runBenchmark(argv.size(), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "End running BenchmarkRunner with status: '%d'", status);
    app->activity->vm->DetachCurrentThread();
}
