include(FetchContent)

set(GIT_REPOSITORY_xyts_strategy_sdk "git@github.com:xywanz/xyts-strategy-sdk.git")
message("GIT_REPOSITORY_xyts_strategy_sdk=${GIT_REPOSITORY_xyts_strategy_sdk}")

FetchContent_Declare(xyts_strategy_sdk GIT_REPOSITORY "${GIT_REPOSITORY_xyts_strategy_sdk}"
                                       GIT_TAG main
                                       SOURCE_DIR "${PROJECT_SOURCE_DIR}/xyts-strategy-sdk")
FetchContent_MakeAvailable(xyts_strategy_sdk)
