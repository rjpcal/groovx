Face::Face
Fish::Fish
FixPt::FixPt
Gabor::Gabor
Gtext::Gtext "Hello, World!"
MaskHatch::MaskHatch
MorphyFace::MorphyFace

Pos::Pos
Jitter::Jitter

EventRh::EventRh
NullRh::NullRh

Block::Block
Block::Block

puts "saving..."
set save_time [ time { Expt::save expt.test } 20 ]
puts "save time $save_time"

puts "writing..."
set write_time [time { Expt::write expt.xpt } 20 ]
puts "write time $write_time"

puts "loading..."
set load_time [ time { Expt::load expt.test } 20 ]
puts "load time $load_time"

puts "reading..."
set read_time [time { Expt::read expt.xpt } 20 ]
puts "read time $read_time"

exit
