#pragma once

#include <map>
#include <string>

#include <QString>

enum class ButtonType {
    kInit,
    kFocus,
    kField,
    kWrong,
    kCorrect
};

std::map<ButtonType, QString> ButtonStyles {
    {ButtonType::kInit, "background-color: darkGray;" 
                        "color: Black;"
                        "border-radius: 0px"},

    {ButtonType::kField, "background-color: white;" 
                         "color: Blue;"
                         "border-radius: 0px" },

    {ButtonType::kFocus, "background-color: blue;"
                         "border-radius: 0px"},

    {ButtonType::kWrong, "background-color: red;"
                         "border-radius: 0px"},

    {ButtonType::kCorrect, "background-color: green;"
                           "border-radius: 0px" },
};