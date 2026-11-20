//! The safe API without a device: versions, plugins, errors and enums.
use jsr_sdk::{api_version, library_version, ErrorCode, Manager, ReceiverMode, API_VERSION};

fn assert_send<T: Send>() {}

#[test]
fn versions_match_the_crate() {
    assert_eq!(api_version().unwrap(), API_VERSION);
    assert!(!library_version().unwrap().is_empty());
}

#[test]
fn manager_is_send() {
    assert_send::<Manager>();
}

#[test]
fn plugins_load_and_describe_themselves() {
    let manager = Manager::new().unwrap();
    manager.load_plugins_from_binary_dir().unwrap();
    let names = manager.get_plugin_names().unwrap();
    assert!(!names.is_empty(), "no plugins next to the test program");
    assert!(names.iter().any(|name| name == "JSRDotNET_DiagLib"));
    assert!(!manager.get_plugin_path().unwrap().is_empty());

    let metadata = manager.get_plugin_library_metadata(&names[0]).unwrap();
    assert_eq!(metadata.name, names[0]);
}

#[test]
fn failure_is_an_error_with_code_and_message() {
    let manager = Manager::new().unwrap();
    manager.load_plugins_from_binary_dir().unwrap();
    let error = manager
        .set_current_pulser_receiver_by_model("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0)
        .unwrap_err();
    assert_eq!(error.code(), ErrorCode::NotFound);
    assert!(error.message().contains("NO-SUCH-MODEL"), "{error}");
}

#[test]
fn nul_in_a_string_is_an_invalid_argument() {
    let manager = Manager::new().unwrap();
    let error = manager.add_managed_plugin("bad\0name").unwrap_err();
    assert_eq!(error.code(), ErrorCode::InvalidArgument);
}

#[test]
fn managers_can_be_recreated() {
    for _ in 0..2 {
        let manager = Manager::new().unwrap();
        manager.load_plugins_from_binary_dir().unwrap();
        drop(manager);
    }
}

#[test]
fn enums_convert_both_ways() {
    assert_eq!(i32::from(ReceiverMode::Thru), 1);
    assert_eq!(ReceiverMode::from(0), ReceiverMode::Echo);
    assert_eq!(ReceiverMode::from(42), ReceiverMode::Unrecognized(42));
    assert_eq!(i32::from(ReceiverMode::Unrecognized(42)), 42);
    assert_eq!(ErrorCode::from(-984), ErrorCode::NullPointer);
}
