hi_freq = 500;
mid_freq = 350;
lo_freq = 200;
dur = 0.3;
samp_freq = 44100;

taper_width = 75;
taper = [linspace(0,1,taper_width)...
		ones(1,dur*samp_freq-2*taper_width)...
		linspace(1,0,taper_width)];

snd = taper .* sinwave(hi_freq, dur, samp_freq);
auwrite(snd, samp_freq, strcat('sin_', num2str(hi_freq), 'Hz_',...
	 num2str(dur*1000), 'ms.au') );
snd = taper .* sinwave(mid_freq, dur, samp_freq);
auwrite(snd, samp_freq, strcat('sin_', num2str(mid_freq), 'Hz_',...
	 num2str(dur*1000), 'ms.au') );
snd = taper .* sinwave(lo_freq, dur, samp_freq);
auwrite(snd, samp_freq, strcat('sin_', num2str(lo_freq), 'Hz_',...
	 num2str(dur*1000), 'ms.au') );

width = .25;
snd = taper .* sawwave(hi_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(hi_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(mid_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(mid_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(lo_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(lo_freq), 'Hz_', num2str(dur*1000), 'ms.au') );

width = .50;
snd = taper .* sawwave(hi_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(hi_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(mid_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(mid_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(lo_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(lo_freq), 'Hz_', num2str(dur*1000), 'ms.au') );

width = .75;
snd = taper .* sawwave(hi_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(hi_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(mid_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(mid_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(lo_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(lo_freq), 'Hz_', num2str(dur*1000), 'ms.au') );

width = 1.0;
snd = taper .* sawwave(hi_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(hi_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(mid_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(mid_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
snd = taper .* sawwave(lo_freq, dur, samp_freq, width);
auwrite(snd, samp_freq, strcat('saw', num2str(width*100), '_',...
	 num2str(lo_freq), 'Hz_', num2str(dur*1000), 'ms.au') );
