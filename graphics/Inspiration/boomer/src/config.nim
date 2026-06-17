import strutils

type Config* = object
  min_scale*: float32
  scroll_speed*: float32
  drag_friction*: float32
  scale_friction*: float32

const defaultConfig* = Config(
  min_scale: 0.01'f32,
  scroll_speed: 1.0'f32,
  drag_friction: 6.0'f32,
  scale_friction: 4.0'f32,
)

proc loadConfig*(filePath: string): Config =
  result = defaultConfig
  for rawLine in filePath.lines:
    let line = rawLine.strip
    if line.len == 0 or line[0] == '#':
      continue
    let pair = line.split('=', 1)
    if pair.len < 2:
      continue
    let key = pair[0].strip
    let value = pair[1].strip
    case key
    of "min_scale":
      result.min_scale = parseFloat(value).float32
    of "scroll_speed":
      result.scroll_speed = parseFloat(value).float32
    of "drag_friction":
      result.drag_friction = parseFloat(value).float32
    of "scale_friction":
      result.scale_friction = parseFloat(value).float32
    else:
      quit "Unknown config key `$#`" % [key]

proc generateDefaultConfig*(filePath: string) =
  var f = open(filePath, fmWrite)
  defer: f.close
  f.write("min_scale = ", defaultConfig.min_scale, "\n")
  f.write("scroll_speed = ", defaultConfig.scroll_speed, "\n")
  f.write("drag_friction = ", defaultConfig.drag_friction, "\n")
  f.write("scale_friction = ", defaultConfig.scale_friction, "\n")
