#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "game.hpp"

#include <board.hpp>
#include <shared.hpp>

#include <multi_game.hpp>

namespace py = pybind11;

PYBIND11_MODULE(Tetris_AGENT, m)
{
    m.doc() = "Tetris module";

    py::enum_<CellState>(m, "CellState")
        .value("EMPTY", CellState::EMPTY)
        .value("SPAWN", CellState::SPAWN)
        .value("ACTIVE", CellState::ACTIVE)
        .value("FILLED_I", CellState::FILLED_I)
        .value("FILLED_O", CellState::FILLED_O)
        .value("FILLED_T", CellState::FILLED_T)
        .value("FILLED_S", CellState::FILLED_S)
        .value("FILLED_Z", CellState::FILLED_Z)
        .value("FILLED_J", CellState::FILLED_J)
        .value("FILLED_L", CellState::FILLED_L)
        .export_values();

    py::enum_<Actions>(m, "Actions")
        .value("MOVE_LEFT", Actions::MOVE_LEFT)
        .value("MOVE_RIGHT", Actions::MOVE_RIGHT)
        .value("SOFT_DROP", Actions::SOFT_DROP)
        .value("ROTATE_CLOCKWISE", Actions::ROTATE_CLOCKWISE)
        .value("ROTATE_COUNTER_CLOCKWISE", Actions::ROTATE_COUNTER_CLOCKWISE)
        .value("HOLD", Actions::HOLD)
        .value("HARD_DROP", Actions::HARD_DROP)
        .export_values();

    py::enum_<PreciseActions>(m, "PreciseActions")
        .value("ROTATION_0_COLUMN_0", PreciseActions::ROTATION_0_COLUMN_0)
        .value("ROTATION_1_COLUMN_0", PreciseActions::ROTATION_1_COLUMN_0)
        .value("ROTATION_2_COLUMN_0", PreciseActions::ROTATION_2_COLUMN_0)
        .value("ROTATION_3_COLUMN_0", PreciseActions::ROTATION_3_COLUMN_0)
        .value("ROTATION_0_COLUMN_1", PreciseActions::ROTATION_0_COLUMN_1)
        .value("ROTATION_1_COLUMN_1", PreciseActions::ROTATION_1_COLUMN_1)
        .value("ROTATION_2_COLUMN_1", PreciseActions::ROTATION_2_COLUMN_1)
        .value("ROTATION_3_COLUMN_1", PreciseActions::ROTATION_3_COLUMN_1)
        .value("ROTATION_0_COLUMN_2", PreciseActions::ROTATION_0_COLUMN_2)
        .value("ROTATION_1_COLUMN_2", PreciseActions::ROTATION_1_COLUMN_2)
        .value("ROTATION_2_COLUMN_2", PreciseActions::ROTATION_2_COLUMN_2)
        .value("ROTATION_3_COLUMN_2", PreciseActions::ROTATION_3_COLUMN_2)
        .value("ROTATION_0_COLUMN_3", PreciseActions::ROTATION_0_COLUMN_3)
        .value("ROTATION_1_COLUMN_3", PreciseActions::ROTATION_1_COLUMN_3)
        .value("ROTATION_2_COLUMN_3", PreciseActions::ROTATION_2_COLUMN_3)
        .value("ROTATION_3_COLUMN_3", PreciseActions::ROTATION_3_COLUMN_3)
        .value("ROTATION_0_COLUMN_4", PreciseActions::ROTATION_0_COLUMN_4)
        .value("ROTATION_1_COLUMN_4", PreciseActions::ROTATION_1_COLUMN_4)
        .value("ROTATION_2_COLUMN_4", PreciseActions::ROTATION_2_COLUMN_4)
        .value("ROTATION_3_COLUMN_4", PreciseActions::ROTATION_3_COLUMN_4)
        .value("ROTATION_0_COLUMN_5", PreciseActions::ROTATION_0_COLUMN_5)
        .value("ROTATION_1_COLUMN_5", PreciseActions::ROTATION_1_COLUMN_5)
        .value("ROTATION_2_COLUMN_5", PreciseActions::ROTATION_2_COLUMN_5)
        .value("ROTATION_3_COLUMN_5", PreciseActions::ROTATION_3_COLUMN_5)
        .value("ROTATION_0_COLUMN_6", PreciseActions::ROTATION_0_COLUMN_6)
        .value("ROTATION_1_COLUMN_6", PreciseActions::ROTATION_1_COLUMN_6)
        .value("ROTATION_2_COLUMN_6", PreciseActions::ROTATION_2_COLUMN_6)
        .value("ROTATION_3_COLUMN_6", PreciseActions::ROTATION_3_COLUMN_6)
        .value("ROTATION_0_COLUMN_7", PreciseActions::ROTATION_0_COLUMN_7)
        .value("ROTATION_1_COLUMN_7", PreciseActions::ROTATION_1_COLUMN_7)
        .value("ROTATION_2_COLUMN_7", PreciseActions::ROTATION_2_COLUMN_7)
        .value("ROTATION_3_COLUMN_7", PreciseActions::ROTATION_3_COLUMN_7)
        .value("ROTATION_0_COLUMN_8", PreciseActions::ROTATION_0_COLUMN_8)
        .value("ROTATION_1_COLUMN_8", PreciseActions::ROTATION_1_COLUMN_8)
        .value("ROTATION_2_COLUMN_8", PreciseActions::ROTATION_2_COLUMN_8)
        .value("ROTATION_3_COLUMN_8", PreciseActions::ROTATION_3_COLUMN_8)
        .value("ROTATION_0_COLUMN_9", PreciseActions::ROTATION_0_COLUMN_9)
        .value("ROTATION_1_COLUMN_9", PreciseActions::ROTATION_1_COLUMN_9)
        .value("ROTATION_2_COLUMN_9", PreciseActions::ROTATION_2_COLUMN_9)
        .value("ROTATION_3_COLUMN_9", PreciseActions::ROTATION_3_COLUMN_9)
        .value("HOLD", PreciseActions::HOLD)
        .export_values();

    py::class_<StepData>(m, "StepData")
        .def_readwrite("reward", &StepData::reward)
        .def_readwrite("piece_placed", &StepData::piece_placed)
        .def_readwrite("lost", &StepData::lost);

    py::class_<MultiGame>(m, "MultiGame")
        .def(py::init<int, float, float, float, int>())

        .def("initGraphics", &MultiGame::initGraphics, "Initializes window")
        .def("render", &MultiGame::render, "Renders the board")
        .def("closeGraphics", &MultiGame::closeGraphics, "Closes window")

        .def("stepAll", &MultiGame::stepAll, "Runs a step on all the enviroments")
        .def("lookAheadAll", &MultiGame::lookAheadAll, "Looks ahead for best action to take")

        .def("resetAll", &MultiGame::resetAll, "Restarts all of the environments")
        .def("resetThis", &MultiGame::resetThis, "Reset a specific environment")

        .def("getLinesCleared", &MultiGame::getLinesCleared, "Lines cleared")
        .def("getAllClearCounts", &MultiGame::getAllClearCounts, "Clear Counts")

        .def("getStates", &MultiGame::getStates, "Gets the state of all the environments");
}
