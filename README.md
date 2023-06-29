# O3DE PopcornFX Examples

**O3DE** project to showcase **PopcornFX** gem features.
* **PopcornFX:** `2.15`
* **O3DE:** `23.05.0`

This example project presents integration features of PopcornFX into O3DE,
it does not present the full range of PopcornFX features.

Take a tour in the `/Levels/` to explore this project.

The source PopcornFX Project is located in `Assets/ContentExamples/`, you can inspect how effects were created in **[PopcornFX Editor](https://www.popcornfx.com/popcornfx-editor/)**.

If you have any questions, suggestions, or just feed backs, you can
use **Github Issues** !

# Setup

This project requires the PopcornFX Gem for O3DE.

## If you are cloning this project from Github

The gem is already present as a `submodule` in `Gems/PopcornFX`.

Simply run the following to clone this project and the PopcornFX Gem submodule:

    git clone --recursive REPO_URL

If you already cloned the repo, you can run:

    git submodule update --init --recursive

## If you downloaded a .zip archive

The gem is already present in packaged archives, there's nothing to do in this case.

## If you installed the plugin outside this project

O3DE supports per-project or globally installed gems. If PopcornFX Gem for O3DE is already installed, you can remove the `Gems/PopcornFX` folder and modify the `project.json` with the path of the already installed gem.

**Note:** make sure that your installed PopcornFX Gem version matches the same version as stated above.

## Compiling the project

To launch this project on Windows, build it with the O3DE Project Manager and launch the Editor.

**Note:** Although the PopcornFX Gem for O3DE supports multiple platforms, this project was only tested on Windows.

# Quick Links: Documentation and Support

* [**Plugin** documentation](https://www.popcornfx.com/docs/popcornfx-v2/plugins/o3de-gem/) (Install, Import, Setup, Troubleshooting, etc..)
* [PopcornFX **Editor** documentation](https://www.popcornfx.com/docs/popcornfx-v2/)
* [PopcornFX **Discord** server](https://discord.gg/4ka27cVrsf)

# License

See [LICENSE.md](/LICENSE.md).