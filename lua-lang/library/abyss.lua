-- ---------------------------------------------------------------------- --
-- THIS FILE WAS AUTO-GENERATED BY generate.py -  DO NOT HAND MODIFY!     --
-- ---------------------------------------------------------------------- --
---@meta
---version: 0.1

---@class abyss
abyss = {}

---Creates a button
---@param image Image @ The image to use for the button. Needs to be alive at least as long as the button is.
---@return Button @ The created button.
function abyss.createButton(image) end

---Creates a new input listener
---@return InputListener @ The input listener
function abyss.createInputListener() end

---Creates a label
---@param font IFont @ The font to use for the label.
---@return Label @ The created label.
function abyss.createLabel(font) end

---Creates a map renderer
---@param zone Zone @ The zone to render.
---@return MapRenderer @ The created map renderer.
function abyss.createMapRenderer(zone) end

---Creates a new empty node
---@return Node @ The new node
function abyss.createNode() end

---Creates a sprite from the image
---@param image Image @ The image to be used for the sprite. Needs to be alive at least as long as the sprite is.
---@return Sprite @ The created sprite.
function abyss.createSprite(image) end

---Adds a loader provider
---@param providerType string @ The type of provider to add ('mpq', 'casc', 'filesystem')
---@param providerPath string @ No description set in scripthost.h:75.
function abyss.addLoaderProvider(providerType, providerPath) end

---Creates a palette
---@param paletteName string @ The name of the palette to create.
---@param path string @ The path to the palette file.
function abyss.createPalette(paletteName, path) end

---Creates a sound effect
---@param fileName string @ The name of the file to load.
---@return SoundEffect @ The created sound effect.
function abyss.createSoundEffect(fileName) end

---Creates a TTF font
---@param fontPath string @ The path to the font file.
---@param size number @ The size of the font.
---@param hinting string @ The hinting to use for the font ('light', 'mono', 'normal', 'none').
---@return TtfFont @ No description set in scripthost.h:133.
function abyss.createTtfFont(fontPath, size, hinting) end

---Gets the build type of the platform the engine was built for
---@return string @ The build type
function abyss.getBuildType() end

---Gets the CPU type of the engine
---@return string @ The CPU type
function abyss.getCpuType() end

---Gets the name of the platform the engine was built for
---@return string @ The platform name
function abyss.getPlatform() end

---Gets the current screen size
---@return number, number @ The width/height screen size
function abyss.getScreenSize() end

---Gets the engine version
---@return number, number @ The engine version
function abyss.getEngineVersion() end

---Determines if a key was pressed
---@param scancode number @ The scancode to test.
---@return boolean @ True if the key is pressed, false otherwise.
function abyss.isKeyPressed(scancode) end

---Loads a DS1 stamp
---@param fileName string @ The name of the file to load.
---@return DS1 @ The created DS1 stamp.
function abyss.loadDS1(fileName) end

---Loads an image from file
---@param spritePath string @ The path to the sprite file.
---@param paletteName string @ The name of the palette to use for the sprite.
---@return Image @ The loaded image.
function abyss.loadImage(spritePath, paletteName) end

---Loads a text file
---@param filePath string @ The path to the file to load.
---@return string @ The text of the file.
function abyss.loadString(filePath) end

---Loads a .tbl file
---@param filePath string @ The path to the file to load.
---@return function @ The lua table. The string keys are exposed as is, the numeric keys as '#1234'
function abyss.loadTbl(filePath) end

---Creates a zone (map/game area)
---@return Zone @ The created zone.
function abyss.createZone() end

---Determines if a file exists
---@param fileName string @ The name of the file to check.
---@return boolean @ True if the file exists, false otherwise.
function abyss.fileExists(fileName) end

---Gets a configuration value
---@param category string @ The category that the variable is defined under.
---@param value string @ The name of the variable who's value you want to get.
---@return string @ The value of the specified category and variable.
function abyss.getConfig(category, value) end

---Logs a message
---@param level string @ The log level ('debug', 'info', 'warn', 'error', 'fatal')
---@param message string @ The message to log.
function abyss.log(level, message) end

---Converts orthographic coordinates to world coordinates
---@param x number @ The ortho X coordinate.
---@param y number @ The ortho Y coordinate.
---@return number, number @ The X/Y world coordinates.
function abyss.orthoToWorld(x, y) end

---Panics and terminates the engine
---@param message string @ The message to display
function abyss.panic(message) end

---Plays looping background music
---@param fileName string @ The name of the file to play.
function abyss.playBackgroundMusic(fileName) end

---Plays a video
---@param videoPath string @ The path to the video file.
---@param callback function @ The callback to call when the video is finished.
function abyss.playVideo(videoPath, callback) end

---Plays video and audio (for videos with separate audio tracks)
---@param videoPath string @ The path to the video file.
---@param audioPath string @ The path to the audio file.
---@param callback function @ The callback to call when the video is finished.
function abyss.playVideoAndAudio(videoPath, audioPath, callback) end

---Resets the mouse button state
function abyss.resetMouseState() end

---Sets the cursor sprite and offsets
---@param sprite Sprite @ The sprite to use for the cursor.
---@param offsetX number @ The X offset of the cursor.
---@param offsetY number @ The Y offset of the cursor.
function abyss.setCursor(sprite, offsetX, offsetY) end

---Sets the window title
---@param title string @ No description set in scripthost.h:171.
function abyss.setWindowTitle(title) end

---Sets the visibility of the system cursor
---@param show boolean @ The visibility of the system cursor.
function abyss.showSystemCursor(show) end

---Shuts down the engine
function abyss.shutdown() end

---Converts UTF16 to UTF8
---@param str string @ The source string.
---@return string @ The string in UTF8 format.
function abyss.utf16To8(str) end

---Converts world coordinates to orthographic coordinates
---@param x number @ The world X coordinate.
---@param y number @ The world Y coordinate.
---@return number, number @ The X/Y ortho coordinates.
function abyss.worldToOrtho(x, y) end
