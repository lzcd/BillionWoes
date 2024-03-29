# Billion Woes

One man's weekend project to scratch a very random programming itch.

## What is it?

Inspired by others' attempts at the [1 Billion Row Challenge](https://github.com/gunnarmorling/1brc), I thought I'd have a stab at it... in C...despite not being much of a C developer.

Anyway...

It works.

It's not the fastest by a sizable margin... but it works.

On my gaming rig it does it in ~200 seconds.

(No idea what that translates to on an officially sanctioned machine.)

## Why isn't it the fastest?

No parallelism.
Single core all the way.

## Why isn't it the slowest?

Single pass parsing and a simple area bump style allocator for memory management.

## Will it be better in the future?

Maybe.

The current approach wasn't built to be thread safe so it may involve some drastic surgery.

My allocator doesn't do any form of bucketing so any arena growth requires a malloc and a full mem copy from the older smaller instance. 
Plus pointers don't survive between resizes so coding style takes a hit.