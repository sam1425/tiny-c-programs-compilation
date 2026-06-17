import os

import navigation
import screenshot
import config

import x11/xlib,
       x11/x,
       x11/xutil,
       x11/keysym,
       x11/xrandr,
       x11/cursorfont
import opengl, opengl/glx
import la
import strutils
import math
import options

type Shader = tuple[path, content: string]

proc readShader(file: string): Shader =
  when nimvm:
    result.path = file
    result.content = slurp result.path
  else:
    result.path = "src" / file
    result.content = readFile result.path

when defined(developer):
  var
    vertexShader = readShader "vert.glsl"
    fragmentShader = readShader "frag.glsl"

  proc reloadShader(shader: var Shader) =
    shader.content = readFile shader.path
else:
  const
    vertexShader = readShader "vert.glsl"
    fragmentShader = readShader "frag.glsl"

proc newShader(shader: Shader, kind: GLenum): GLuint =
  result = glCreateShader(kind)
  var shaderArray = allocCStringArray([shader.content])
  glShaderSource(result, 1, shaderArray, nil)
  glCompileShader(result)
  deallocCStringArray(shaderArray)

  var success: GLint
  var infoLog = newString(512).cstring
  glGetShaderiv(result, GL_COMPILE_STATUS, addr success)
  if not success.bool:
    glGetShaderInfoLog(result, 512, nil, infoLog)
    echo "------------------------------"
    echo "Error during shader compilation: ", shader.path, ". Log:"
    echo infoLog
    echo "------------------------------"
    raise newException(ValueError, "Shader compilation failed: " & shader.path)

proc newShaderProgram(vertex, fragment: Shader): GLuint =
  result = glCreateProgram()

  var
    vertexShader = newShader(vertex, GL_VERTEX_SHADER)
    fragmentShader = newShader(fragment, GL_FRAGMENT_SHADER)

  glAttachShader(result, vertexShader)
  glAttachShader(result, fragmentShader)

  glLinkProgram(result)

  glDeleteShader(vertexShader)
  glDeleteShader(fragmentShader)

  var success: GLint
  var infoLog = newString(512).cstring
  glGetProgramiv(result, GL_LINK_STATUS, addr success)
  if not success.bool:
    glGetProgramInfoLog(result, 512, nil, infoLog)
    echo infoLog
    raise newException(ValueError, "Shader linking failed")

  glUseProgram(result)

type Flashlight = object
  isEnabled: bool
  shadow: float32
  radius: float32
  targetRadius: float32
  deltaRadius: float32

const
  INITIAL_FL_DELTA_RADIUS = 250.0
  FL_DELTA_RADIUS_DECELERATION = 10.0

proc update(flashlight: var Flashlight, dt: float32) =
  if abs(flashlight.deltaRadius) > 1.0'f32:
    flashlight.targetRadius = max(10.0'f32, flashlight.targetRadius + flashlight.deltaRadius * dt)
    flashlight.deltaRadius -= flashlight.deltaRadius * FL_DELTA_RADIUS_DECELERATION * dt

  if flashlight.isEnabled:
    flashlight.shadow = min(flashlight.shadow + 6.0'f32 * dt, 0.8'f32)
    flashlight.radius += (flashlight.targetRadius - flashlight.radius) * (1.0'f32 - exp(-15.0'f32 * dt))
  else:
    flashlight.shadow = max(flashlight.shadow - 6.0'f32 * dt, 0.0'f32)
    flashlight.radius += (0.0'f32 - flashlight.radius) * (1.0'f32 - exp(-15.0'f32 * dt))


type
  UniformLocations = object
    cameraPos: GLint
    cameraScale: GLint
    screenshotSize: GLint
    windowSize: GLint
    cursorPos: GLint
    flShadow: GLint
    flRadius: GLint
    laserEnabled: GLint

proc getUniformLocations(shader: GLuint): UniformLocations =
  result.cameraPos = glGetUniformLocation(shader, "cameraPos".cstring)
  result.cameraScale = glGetUniformLocation(shader, "cameraScale".cstring)
  result.screenshotSize = glGetUniformLocation(shader, "screenshotSize".cstring)
  result.windowSize = glGetUniformLocation(shader, "windowSize".cstring)
  result.cursorPos = glGetUniformLocation(shader, "cursorPos".cstring)
  result.flShadow = glGetUniformLocation(shader, "flShadow".cstring)
  result.flRadius = glGetUniformLocation(shader, "flRadius".cstring)
  result.laserEnabled = glGetUniformLocation(shader, "laserEnabled".cstring)

proc draw(screenshot: PXImage, camera: Camera, shader, vao, texture: GLuint,
          windowSize: Vec2f, mouse: Mouse, flashlight: Flashlight,
          locs: UniformLocations, laserEnabled: bool) =
  glClearColor(0.1, 0.1, 0.1, 1.0)
  glClear(GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT)

  glUseProgram(shader)

  glUniform2f(locs.cameraPos, camera.position[0], camera.position[1])
  glUniform1f(locs.cameraScale, camera.scale)
  glUniform2f(locs.screenshotSize, screenshot.width.float32, screenshot.height.float32)
  glUniform2f(locs.windowSize, windowSize.x.float32, windowSize.y.float32)
  glUniform2f(locs.cursorPos, mouse.curr.x.float32, mouse.curr.y.float32)
  glUniform1f(locs.flShadow, flashlight.shadow)
  glUniform1f(locs.flRadius, flashlight.radius)
  glUniform1i(locs.laserEnabled, if laserEnabled: 1 else: 0)

  glBindVertexArray(vao)
  glDrawElements(GL_TRIANGLES, count = 6, GL_UNSIGNED_INT, indices = nil)



proc getCursorPosition(display: PDisplay): Vec2f =
  var root, child: Window
  var root_x, root_y, win_x, win_y: cint
  var mask: cuint
  discard XQueryPointer(
    display, DefaultRootWindow(display),
    addr root, addr child,
    addr root_x, addr root_y,
    addr winX, addr winY,
    addr mask);
  result.x = root_x.float32
  result.y = root_y.float32

proc selectWindow(display: PDisplay): Window =
  var cursor = XCreateFontCursor(display, XC_crosshair)
  defer: discard XFreeCursor(display, cursor)

  var root = DefaultRootWindow(display)
  discard XGrabPointer(display, root, 0,
                       ButtonMotionMask or
                       ButtonPressMask or
                       ButtonReleaseMask,
                       GrabModeAsync, GrabModeAsync,
                       root, cursor,
                       CurrentTime)
  defer: discard XUngrabPointer(display, CurrentTime)

  discard XGrabKeyboard(display, root, 0,
                        GrabModeAsync, GrabModeAsync,
                        CurrentTime)
  defer: discard XUngrabKeyboard(display, CurrentTime)

  var event: XEvent
  while true:
    discard XNextEvent(display, addr event)
    case event.theType
    of ButtonPress:
      return event.xbutton.subwindow
    of KeyPress:
      return root
    else:
      discard

  return root

proc xElevenErrorHandler(display: PDisplay, errorEvent: PXErrorEvent): cint{.cdecl.} =
  const CAPACITY = 256
  var errorMessage: array[CAPACITY, char]
  discard XGetErrorText(display, errorEvent.error_code.cint, cast[cstring](addr errorMessage), CAPACITY)
  echo "X ELEVEN ERROR: ", $(cast[cstring](addr errorMessage))

proc main() =
  let boomerDir = getConfigDir() / "boomer"
  var configFile = boomerDir / "config"
  var windowed = false
  var delaySec = 0.0

  # TODO(#95): Make boomer optionally wait for some kind of event (for example, key press)
  block:
    proc versionQuit() =
      const hash = gorgeEx("git rev-parse HEAD")
      quit "boomer-$#" % [if hash.exitCode == 0: hash.output[0 .. 7] else: "unknown"]
    proc usageQuit() =
      quit """Usage: boomer [OPTIONS]
  -d, --delay <seconds: float>  delay execution of the program by provided <seconds>
  -h, --help                    show this help and exit
      --new-config [filepath]   generate a new default config at [filepath]
  -c, --config <filepath>       use config at <filepath>
  -V, --version                 show the current version and exit
  -w, --windowed                windowed mode instead of fullscreen"""
    var i = 1
    while i <= paramCount():
      let arg = paramStr(i)
      case arg
      of "-d", "--delay":
        if i + 1 > paramCount():
          echo "No value is provided for ", arg
          usageQuit()
        delaySec = parseFloat(paramStr(i + 1))
        i += 2
      of "-w", "--windowed":
        windowed = true
        i += 1
      of "-h", "--help":
        usageQuit()
      of "-V", "--version":
        versionQuit()
      of "--new-config":
        var configName = none(string)
        if i + 1 <= paramCount():
          let nextArg = paramStr(i + 1)
          if nextArg.len > 0 and nextArg[0] != '-':
            configName = some(nextArg)
        
        let newConfigPath = configName.get(configFile)
        createDir(newConfigPath.splitFile.dir)
        if newConfigPath.fileExists:
          stdout.write("File ", newConfigPath, " already exists. Replace it? [yn] ")
          if stdin.readChar != 'y':
            quit "Disaster prevented"
        generateDefaultConfig(newConfigPath)
        quit "Generated config at " & newConfigPath
      of "-c", "--config":
        if i + 1 > paramCount():
          echo "No value is provided for ", arg
          usageQuit()
        configFile = paramStr(i + 1)
        i += 2
      else:
        echo "Unknown flag `", arg, "`"
        usageQuit()
  sleep(floor(delaySec * 1000).int)

  var config = defaultConfig

  if fileExists configFile:
    config = loadConfig(configFile)
  else:
    stderr.writeLine configFile & " doesn't exist. Using default values. "

  echo "Using config: ", config

  var display = XOpenDisplay(nil)
  if display == nil:
    quit "Failed to open display"
  defer:
    discard XCloseDisplay(display)

  discard XSetErrorHandler(xElevenErrorHandler)

  when defined(select):
    echo "Please select window:"
    var trackingWindow = selectWindow(display)
  else:
    var trackingWindow = DefaultRootWindow(display)

  var screenConfig = XRRGetScreenInfo(display, DefaultRootWindow(display))
  let rate = XRRConfigCurrentRate(screenConfig)
  XRRFreeScreenConfigInfo(screenConfig)
  echo "Screen rate: ", rate

  let screen = XDefaultScreen(display)
  var glxMajor, glxMinor: cint

  if (not glXQueryVersion(display, glxMajor, glxMinor).bool or
      (glxMajor == 1.cint and glxMinor < 3.cint) or
      (glxMajor < 1.cint)):
    quit "Invalid GLX version. Expected >=1.3"
  echo("GLX version ", glxMajor, ".", glxMinor)
  echo("GLX extension: ", glXQueryExtensionsString(display, screen))

  var attrs = [
    GLX_RGBA,
    GLX_DEPTH_SIZE, 24,
    GLX_DOUBLEBUFFER,
    None
  ]

  var vi = glXChooseVisual(display, 0, addr attrs[0])
  if vi == nil:
    quit "No appropriate visual found"

  echo "Visual ", vi.visualid, " selected"
  var swa: XSetWindowAttributes
  swa.colormap = XCreateColormap(display, DefaultRootWindow(display),
                                 vi.visual, AllocNone)
  swa.event_mask = ButtonPressMask or ButtonReleaseMask or
                   KeyPressMask or KeyReleaseMask or
                   PointerMotionMask or ExposureMask or ClientMessage or
                   FocusChangeMask or StructureNotifyMask
  if not windowed:
    swa.override_redirect = 1
    swa.save_under = 1

  var attributes: XWindowAttributes
  discard XGetWindowAttributes(
    display,
    DefaultRootWindow(display),
    addr attributes)
  var win = XCreateWindow(
    display, DefaultRootWindow(display),
    0, 0, attributes.width.cuint, attributes.height.cuint, 0,
    vi.depth, InputOutput, vi.visual,
    CWColormap or CWEventMask or CWOverrideRedirect or CWSaveUnder, addr swa)

  discard XMapWindow(display, win)
  if not windowed:
    discard XSetInputFocus(display, win, RevertToParent, CurrentTime)

  var windowWidth = attributes.width.cint
  var windowHeight = attributes.height.cint


  var wmName = "boomer"
  var wmClass = "Boomer"
  var hints = XClassHint(res_name: cstring(wmName), res_class: cstring(wmClass))

  discard XStoreName(display, win, cstring(wmName))
  discard XSetClassHint(display, win, addr(hints))

  var wmDeleteMessage = XInternAtom(
    display, "WM_DELETE_WINDOW",
    0.cint)

  discard XSetWMProtocols(display, win,
                          addr wmDeleteMessage, 1)

  var glc = glXCreateContext(display, vi, nil, GL_TRUE.cint)
  discard XFree(vi)
  discard glXMakeCurrent(display, win, glc)

  loadExtensions()

  var shaderProgram = newShaderProgram(vertexShader, fragmentShader)
  var locs = getUniformLocations(shaderProgram)

  var screenshot = newScreenshot(display, trackingWindow)
  defer: screenshot.destroy(display)

  var
    vao, vbo, ebo: GLuint
    vertices = [
      # Position                 Texture coords
      [GLfloat  1.0,   0.0, 0.0, 1.0, 1.0], # Top right
      [GLfloat  1.0,   1.0, 0.0, 1.0, 0.0], # Bottom right
      [GLfloat  0.0,   1.0, 0.0, 0.0, 0.0], # Bottom left
      [GLfloat  0.0,   0.0, 0.0, 0.0, 1.0]  # Top left
    ]
    indices = [GLuint(0), 1, 3,
                      1,  2, 3]

  glGenVertexArrays(1, addr vao)
  glGenBuffers(1, addr vbo)
  glGenBuffers(1, addr ebo)
  defer:
    glDeleteVertexArrays(1, addr vao)
    glDeleteBuffers(1, addr vbo)
    glDeleteBuffers(1, addr ebo)

  glBindVertexArray(vao)

  glBindBuffer(GL_ARRAY_BUFFER, vbo)
  glBufferData(GL_ARRAY_BUFFER, size = GLsizeiptr(sizeof(vertices)),
               addr vertices, GL_STATIC_DRAW)

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size = GLsizeiptr(sizeof(indices)),
               addr indices, GL_STATIC_DRAW);

  var stride = GLsizei(vertices[0].len * sizeof(GLfloat))

  glVertexAttribPointer(0, 3, cGL_FLOAT, false, stride, cast[pointer](0))
  glEnableVertexAttribArray(0)

  glVertexAttribPointer(1, 2, cGL_FLOAT, false, stride, cast[pointer](3 * sizeof(GLfloat)))
  glEnableVertexAttribArray(1)

  var texture = 0.GLuint
  glGenTextures(1, addr texture)
  glActiveTexture(GL_TEXTURE0)
  glBindTexture(GL_TEXTURE_2D, texture)

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB.GLint,
               screenshot.image.width,
               screenshot.image.height,
               0,
               # TODO(#13): the texture format is hardcoded
               GL_BGRA,
               GL_UNSIGNED_BYTE,
               screenshot.image.data)
  glGenerateMipmap(GL_TEXTURE_2D)

  glUniform1i(glGetUniformLocation(shaderProgram, "tex".cstring), 0)

  glEnable(GL_TEXTURE_2D)

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER)

  var
    quitting = false
    camera = Camera(scale: 1.0'f32)
    mouse: Mouse =
      block:
        let pos = getCursorPosition(display)
        Mouse(curr: pos, prev: pos)
    flashlight = Flashlight(
      isEnabled: false,
      radius: 0.0'f32,
      targetRadius: 100.0'f32)
    laserEnabled = false



  let dt = 1.0'f32 / rate.float32
  var originWindow: Window
  var revertToReturn: cint
  discard XGetInputFocus(display, addr originWindow, addr revertToReturn)
  var currentTexWidth = screenshot.image.width
  var currentTexHeight = screenshot.image.height

  while not quitting:
    glViewport(0, 0, windowWidth, windowHeight)

    var xev: XEvent
    while XPending(display) > 0:
      discard XNextEvent(display, addr xev)

      proc scrollUp() =
        if (xev.xkey.state and ControlMask) > 0.uint32 and flashlight.isEnabled:
          flashlight.deltaRadius -= INITIAL_FL_DELTA_RADIUS
        else:
          camera.deltaScale += config.scroll_speed
          camera.scalePivot = mouse.curr

      proc scrollDown() =
        if (xev.xkey.state and ControlMask) > 0.uint32 and flashlight.isEnabled:
          flashlight.deltaRadius += INITIAL_FL_DELTA_RADIUS
        else:
          camera.deltaScale -= config.scroll_speed
          camera.scalePivot = mouse.curr

      case xev.theType
      of Expose:
        discard

      of MotionNotify:
        mouse.curr = vec2(xev.xmotion.x.float32,
                          xev.xmotion.y.float32)

        if mouse.drag:
          let delta = world(camera, mouse.prev) - world(camera, mouse.curr)
          camera.position += delta
          # delta is the distance the mouse traveled in a single
          # frame. To turn the velocity into units/second we need to
          # multiple it by FPS.
          camera.velocity = delta * rate.float32

        mouse.prev = mouse.curr

      of ClientMessage:
        if cast[Atom](xev.xclient.data.l[0]) == wmDeleteMessage:
          quitting = true

      of ConfigureNotify:
        windowWidth = xev.xconfigure.width
        windowHeight = xev.xconfigure.height

      of FocusOut:
        if not windowed:
          discard XSetInputFocus(display, win, RevertToParent, CurrentTime)

      of KeyPress:
        var key = XLookupKeysym(cast[PXKeyEvent](xev.addr), 0)
        case key
        of XK_EQUAL: scrollUp()
        of XK_MINUS: scrollDown()
        of XK_0:
          camera.scale = 1.0'f32
          camera.deltaScale = 0.0'f32
          camera.position = vec2(0.0'f32, 0.0'f32)
          camera.velocity = vec2(0.0'f32, 0.0'f32)
        of XK_q, XK_Escape:
          quitting = true
        of XK_r:
          if configFile.len > 0 and fileExists(configFile):
            config = loadConfig(configFile)

          when defined(developer):
            if (xev.xkey.state and ControlMask) > 0.uint32:
              echo "------------------------------"
              echo "RELOADING SHADERS"
              try:
                reloadShader(vertexShader)
                reloadShader(fragmentShader)
                let newShaderProgram = newShaderProgram(vertexShader, fragmentShader)
                glDeleteProgram(shaderProgram)
                shaderProgram = newShaderProgram
                locs = getUniformLocations(shaderProgram)
                echo "Shader program ID: ", shaderProgram
              except CatchableError:
                echo "Could not reload the shaders"
              echo "------------------------------"

        of XK_f:
          if (xev.xkey.state and ControlMask) > 0.uint32:
            flashlight.isEnabled = not flashlight.isEnabled
        of XK_l:
          if (xev.xkey.state and ControlMask) > 0.uint32:
            laserEnabled = not laserEnabled
        else:
          discard
      of KeyRelease:
        discard

      of ButtonPress:
        case xev.xbutton.button
        of Button1:
          mouse.prev = mouse.curr
          mouse.drag = true
          camera.velocity = vec2(0.0'f32, 0.0'f32)
        of Button4: scrollUp()
        of Button5: scrollDown()
        else:
          discard

      of ButtonRelease:
        case xev.xbutton.button
        of Button1:
          mouse.drag = false
        else:
          discard
      else:
          discard

    # Query real-time cursor position to eliminate queue latency
    let latestMouse = getCursorPosition(display)
    mouse.curr = latestMouse
    if not mouse.drag:
      mouse.prev = latestMouse

    camera.update(config, dt, mouse, vec2(windowWidth.float32, windowHeight.float32))
    flashlight.update(dt)

    screenshot.image.draw(camera, shaderProgram, vao, texture,
                          vec2(windowWidth.float32, windowHeight.float32),
                          mouse, flashlight, locs, laserEnabled)




    glXSwapBuffers(display, win)

    when defined(live):
      screenshot.refresh(display, trackingWindow)
      if screenshot.image.width != currentTexWidth or
         screenshot.image.height != currentTexHeight:
        currentTexWidth = screenshot.image.width
        currentTexHeight = screenshot.image.height
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB.GLint,
                     currentTexWidth,
                     currentTexHeight,
                     0,
                     GL_BGRA,
                     GL_UNSIGNED_BYTE,
                     screenshot.image.data)
        glGenerateMipmap(GL_TEXTURE_2D)
      else:
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0, 0,
                        currentTexWidth,
                        currentTexHeight,
                        GL_BGRA,
                        GL_UNSIGNED_BYTE,
                        screenshot.image.data)

  # Cleanup OpenGL and X11 resources
  glDeleteTextures(1, addr texture)
  glDeleteProgram(shaderProgram)
  discard glXMakeCurrent(display, None, nil)
  glXDestroyContext(display, glc)
  discard XDestroyWindow(display, win)

  discard XSetInputFocus(display, originWindow, RevertToParent, CurrentTime);
  discard XSync(display, 0)

main()
