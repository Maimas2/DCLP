# Dominion CLI and GUI Card Creator
This is a tool to create custom cards for the game Dominion, created by Donald X. Vaccarino, published by Rio Grande Games.

Run with `--help` for more info and directions on how to use.

This is **not** intended as a replacement for the online Shard of Honor Dominion card creator. This is more intended for more automated tasks, like on a server, or to take advantage of greater hardware, like gpus. However, if you are looking for efficiency, *this is not for you.* The way this works is absolutely not efficient, and should not be treated like it is.

# License
This is licensed under the GNU General Public License v3.0. See `LICENSE` or [the GNU website](https://www.gnu.org/licenses/gpl-3.0.en.html) for more information.

## Card designs
**ALL** card designs are property of Donald X. Vaccarino and Rio Grande Games, not me.

All designs of cards are taken from shardofhonor's similar card creator, found at https://shardofhonor.github.io/dominion-card-generator/.

# Other notes
Unless specified with --high-res, all images (excluding the coin, debt, and VP token) are rendered at half resolution for better load times and performance.
## Curl
This program uses Curl for downloading images. It's website can be found [here](https://curl.se/), and it's license can be found [here](https://curl.se/docs/copyright.html), or by running the program with `--curl-license`.