function snd = sinwave(freq, secs, sample_freq, phase, amp)

if nargin < 4
  phase = 0;
end

if nargin < 5
  amp = 1;
end

num_samples = secs * sample_freq;
t = (0:(num_samples-1)) ./ sample_freq;
snd = amp .* sin(phase + 2*pi*freq*t);
snd(snd > 1) = 1;
snd(snd < -1) = -1;
