cmake_minimum_required(VERSION 3.28)
project(qt_programming_2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(qt_programming_2024 src/main.cpp
        src/MyGame.cpp
        src/MyGame.h
        src/Items/Item.cpp
        src/Items/Item.h
        src/Items/Characters/Character.cpp
        src/Items/Characters/Character.h
        src/Scenes/Scene.cpp
        src/Scenes/Scene.h
        src/Scenes/BattleScene.cpp
        src/Scenes/BattleScene.h
        assets/assets.qrc
        src/Items/Maps/Map.cpp
        src/Items/Maps/Map.h
        src/Items/Maps/Battlefield.cpp
        src/Items/Maps/Battlefield.h
        src/Items/Maps/Platform.cpp
        src/Items/Maps/Platform.h
        src/Items/Mountable.cpp
        src/Items/Mountable.h
        src/Items/Bar/HealthBar.cpp
        src/Items/Bar/HealthBar.h
        src/Items/Bar/WeaponBar.cpp
        src/Items/Bar/WeaponBar.h
        src/Items/Weapons/Weapon.cpp
        src/Items/Weapons/Weapon.h
        src/Items/Weapons/Fist.h
        src/Items/Weapons/Fist.cpp
        src/Items/Weapons/Knife.h
        src/Items/Weapons/Knife.cpp
        src/Items/Weapons/SolidBall.h
        src/Items/Weapons/SolidBall.cpp
        src/Items/Weapons/Rifle.h
        src/Items/Weapons/Rifle.cpp
        src/Items/Weapons/Sniper.h
        src/Items/Weapons/Sniper.cpp
        src/Items/FallingItem.h
        src/Items/FallingItem.cpp
        src/Items/Weapons/Sniper.cpp
)
target_link_libraries(qt_programming_2024
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

