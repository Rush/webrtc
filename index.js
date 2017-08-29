#!/usr/bin/env node

const {spawn} = require('child_process');

const { EventEmitter } = require('events');

const emitter = new EventEmitter;

const split = require('split');

const colors = require('colors/safe');

const activeEnd = spawn('./build/active', [], {
  stdio: 'pipe'
});
const passiveEnd = spawn('bash', ['-c', 'ssh dragon.rushbase.net ./passive'], {
  stdio: 'pipe'
});

activeEnd.on('error', console.error);


activeEnd.stderr.pipe(split()).on('data', line => console.error(colors.red(line)));
passiveEnd.stderr.pipe(split()).on('data', line => console.error(colors.red(line)));

const activeLineEmitter = activeEnd.stdout.pipe(split());
const passiveLineEmitter = passiveEnd.stdout.pipe(split());


activeLineEmitter.once('data', offer => {
  passiveLineEmitter.once('data', answer => {
    console.log('Got answer from passive client');
    setTimeout(() => {
      activeEnd.stdin.write(answer + '\n');
    }, 100);
    
    passiveLineEmitter.on('data', console.log);
    activeLineEmitter.on('data', console.log);
    
  });
  setTimeout(() => {
    passiveEnd.stdin.write(offer + '\n');
    }, 100);
});
