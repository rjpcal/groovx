function snd = sinwave(freq, secs, sample_freq)

num_samples = secs * sample_freq;
x = 0:(num_samples-1);
x = x./sample_freq;
snd = sin(2*pi*freq*x);
