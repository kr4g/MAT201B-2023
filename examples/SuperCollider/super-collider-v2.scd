// Karl Yerkes
// 2022-02-22
// AlloLib + SuperCollider example
Server.default.boot;
(
    g = EZSlider(label: " value ");
    b = NetAddr.new("127.0.0.1", 9010); // Send to AlloLib on this machine
    g.action_({ |ez| b.sendMsg("/Value", ez.value); });
);

