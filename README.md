# OpenGLThings

## What is this
This is a goofy test repository with some random stuff written with `OpenGl` for test and education purposes

> [!WARNING]
> This repository contains bullshit code because author doesn't give a fuck about code prettiness in test projects

## How to run
Install `SDL2` (Native version is `26.5`) and `GLM` (Native version is [`0.9.8`](https://github.com/g-truc/glm/releases/tag/0.9.8.0))

Then setup `GLAD`: 
- Go to the [generator website](https://glad.dav1d.de/)
- Under `API` section set `gl` version to at least `3.3`
- Set `Profile` to `Core`
- Make shure that `Generate a loder` is checked
- Press `GENERATE`
- Download `glad.zip` and unpack it
- Put `glad` and `KHR` folders (In the archive they are in `include` foulder) into your `C++` include directory

Clone the repo and go into it's folder. Then run in console:
```bash
make
make run
```

Or just:
```bash
make full
```

## Credits
Libs used:
- [`SDL2`](https://www.libsdl.org/) for window handling
- [`GLAD`](https://glad.dav1d.de/) for function pointers loading
- [`GLM`](https://glm.g-truc.net/) for linal
- [`STB image`](https://github.com/nothings/stb/blob/master/stb_image.h) for image loading

Skybox images:
- Screenshots from `Minecraft` with [`IterationT`](https://minecraft-inside.ru/shaders/179320-iterationt.html) shader
- [`Planes surface skyboxes`](https://screamingbrainstudios.itch.io/planet-surface-skyboxes)

Weapon assets:
- [Veiny arms](https://share.google/oAHAkMpeHiY432UdZ)
- [Scooter](https://share.google/M7FaUOXzgMgT0C3lq)