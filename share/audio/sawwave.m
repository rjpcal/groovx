function snd = sawwave(freq, secs, sample_freq, width)

num_samples = secs * sample_freq;
x = 0:(num_samples-1);
x = x./sample_freq;
snd = sawtooth(2*pi*freq*x,width);