{
    "Source" : "Basic3DFont.azsl",

    "DepthStencilState" :
    {
        "Depth" :
        {
            "Enable" : true,
            "CompareFunc" : "GreaterEqual"
        }
    },

    "RasterState" :
    {
        "CullMode" : "None"
    },

    "ProgramSettings":
    {
      "EntryPoints":
      [
        {
          "name": "FontVS",
          "type": "Vertex"
        },
        {
          "name": "FontFS",
          "type": "Fragment"
        }
      ]
    },

    "DrawList" : "transparent"
}
