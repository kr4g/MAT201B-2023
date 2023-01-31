// Karl Yerkes
// 2022-02-23
// AlloLib + SuperCollider example

Server.default.boot;

(
// "Numbered" sliders grouped together

var window, allolib;
window = Window("Numbered Sliders", Rect(20, 400, 440, 180));
window.view.decorator = FlowLayout(window.view.bounds);
window.front;
allolib = NetAddr.new("127.0.0.1", 9010);
5.do { |index| var slider;
    slider = EZSlider(window, label: index);
    slider.action_({ |ez| allolib.sendMsg("/numbered/" ++ index, slider.value); });
};
)



(
// "Named" sliders grouped together

var window, allolib, parameter;
window = Window("Named Sliders", Rect(20, 400, 440, 180));
window.view.decorator = FlowLayout(window.view.bounds);
window.front;
allolib = NetAddr.new("127.0.0.1", 9010);
parameter = ["note", "break", "resonance", "pan", "amplitude", "width"];
parameter.do { |name, index| var slider;
    slider = EZSlider(window, label: name);
	slider.action_({ |ez| allolib.sendMsg("/named/" ++ name, slider.value); });
};
)