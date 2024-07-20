#![no_std]
#![feature(lang_items)]

// ========================================
// override panic handler in std
// so that standard library doesn't get linked

#[panic_handler]
fn panic(_: &core::panic::PanicInfo<'_>) -> ! { loop {} }

#[lang = "eh_personality"]
extern "C" fn eh_personality() {}
// ========================================


#[no_mangle]
pub extern "C" fn this_function_is_exported_to_c() {

}
