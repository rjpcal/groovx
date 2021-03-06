function snd = sqrwave(freq, secs, sample_freq, width, phase)

if nargin < 5
  phase = 0;
end

num_samples = secs * sample_freq;
t = (0:(num_samples-1)) ./ sample_freq;
snd = square(phase + 2*pi*freq*t, width*100);
