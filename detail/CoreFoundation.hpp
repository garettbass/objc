#pragma once

namespace OBJC_NAMESPACE {

    library CoreFoundation {"CoreFoundation.framework/CoreFoundation"};

    function_t<long(void*)>
    const CFGetRetainCount { CoreFoundation("CFGetRetainCount") };

    function_t<void(void*)>
    const CFRelease { CoreFoundation("CFRelease") };

    function_t<void*(void*)>
    const CFRetain { CoreFoundation("CFRetain") };

} // namespace OBJC_NAMESPACE