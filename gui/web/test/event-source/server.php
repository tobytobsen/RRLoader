<?php

header('Content-type: text/event-stream');

set_time_limit(0);

for ($i = 0; ; ++$i) {
  print "event: custom-event\n";
  print "data: $i\n\n";
  
  print "data: hallo welt #$i\n\n";
  
  ob_flush();
  flush();
  sleep(1);
}

