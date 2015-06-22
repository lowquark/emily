
function add_button(node, name) node:addbutton{ text = name, margin_t = 5, margin_b = 5, margin_l = 5, margin_r = 5 } end

local mainwindow = muh_ui:addwindow{ text = "window title", x = 50, y = 50, w = 0, h = 0, margin_t = 5, margin_b = 5, margin_l = 5, margin_r = 5, spacing = 5 }
--local content = mainwindow:addvbox{}
mainwindow:addlabel{ text = "I'm the top text! YAYY" }
mainwindow:addlabel{ text = "I'm not the top text! NOOOO  This text can go on forever...." }
local buttons = mainwindow:addhbox{ spacing = 5 }
add_button(buttons, "yay I'm having fun")
add_button(buttons, "Ok")
add_button(buttons, "cancel")
add_button(buttons, "asdfasdf")



