<?php

/** 
 * Basic implementation of a WebSocket Server
 * Copyright 2012 - droptable/murdoc
 */

namespace rrl;

class Server
{
  protected $sock, $conf = [], $pool = [];
  
  public function __construct(array $conf = [])
  {
    require_once 'server/mutex.php';
    
    $conf += [ 'host' => 'localhost', 'port' => 31337 ];
    $this->conf = &$conf;
    
    $this->mutex();
    $this->create();
  }
  
  protected function mutex()
  {
    $mutex = server\mutex_parse();
    
    foreach ($mutex as $idx => $mtx) {
      if ($mtx['host'] === $this->conf['host'] 
       && $mtx['port'] === $this->conf['port']) {
        $this->terminate($mtx['pid']);
        unset($mutex[$idx]);
      }
    }
    
    $mutex[] = [ 
      'host' => $this->conf['host'],
      'port' => $this->conf['port'],
      'pid'  => getmypid(),
      'upt'  => 0
    ];
    
    server\mutex_update($mutex);
  }
  
  // -------------------------------------
  
  protected function create()
  {
    
  }
  
  protected function mutex($path)
  {
    
  }  
  
  protected function terminate($pid) 
  {
    if (PHP_OS === 'WINNT') {
      $procs = (new COM('winmgmts:{impersonationLevel=impersonate}!\.\root\cimv2'))
        ->execQuery('SELECT * FROM Win32_Process WHERE ProcessId="' . $pid .'"');
      
      foreach ($procs as $proc)
        $proc->terminate();
     
      return;
    }
    
    posix_kill($pid, SIGTERM);
  }
}

