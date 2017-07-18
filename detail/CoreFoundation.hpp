#pragma once

namespace OBJC_NAMESPACE {

    library CoreFoundation {"CoreFoundation.framework/CoreFoundation"};

    function_type<long(void*)>
    const CFGetRetainCount { CoreFoundation.function("CFGetRetainCount") };

    function_type<void(void*)>
    const CFRelease { CoreFoundation.function("CFRelease") };

    function_type<void*(void*)>
    const CFRetain { CoreFoundation.function("CFRetain") };

} // namespace OBJC_NAMESPACE