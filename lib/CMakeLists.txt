# project structure
set(Common_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/common/src")
file(GLOB_RECURSE Common_HEADERS "${Common_INCLUDES}/*.hpp")
add_subdirectory(common)

set(Config_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/config/src")
file(GLOB_RECURSE Config_HEADERS "${Config_INCLUDES}/*.hpp")
add_subdirectory(config)

set(Messaging_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/messaging/src")
file(GLOB_RECURSE Threading_HEADERS "${Messaging_INCLUDES}/*.hpp")
add_subdirectory(messaging)

set(Threading_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/threading/src")
file(GLOB_RECURSE Threading_HEADERS "${Threading_INCLUDES}/*.hpp")
add_subdirectory(threading)

set(Bus_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/bus/src")
file(GLOB_RECURSE Bus_HEADERS "${Bus_INCLUDES}/*.hpp")
add_subdirectory(bus)

set(Service_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/service/src")
file(GLOB_RECURSE Service_HEADERS "${Service_INCLUDES}/*.hpp")
add_subdirectory(service)

set(Product_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/product/src")
file(GLOB_RECURSE Product_HEADERS "${Product_INCLUDES}/*.hpp")
add_subdirectory(product)

set(Order_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/order/src")
file(GLOB_RECURSE Order_HEADERS "${Order_INCLUDES}/*.hpp")
add_subdirectory(order)

set(Femas_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/femas/src")
file(GLOB_RECURSE Femas_HEADERS "${Femas_INCLUDES}/*.hpp")
add_subdirectory(femas)

set(Simulation_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/simulation/src")
file(GLOB_RECURSE Simulation_HEADERS "${Simulation_INCLUDES}/*.hpp") 
add_subdirectory(simulation)

set(Arb_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/arb/src")
file(GLOB_RECURSE Arb_HEADERS "${Arb_INCLUDES}/*.hpp") 
add_subdirectory(arb)
