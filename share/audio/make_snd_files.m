function make_snd_files()

% Generate a series of audio files of different pitches and different timbres.

% $Id$

  freqs = [200
           350
           500
           329.6 % E
           349.2 % F
           370.0 % F#
           392.0 % G
           415.3 % G#
           440.0 % A
           466.2 % A#
           493.9 % B
           523.3 % C
           554.4 % C#
           587.3 % D
           622.3 % D#
           659.3 % E
          ];

  samp_freq = 44100;

  freqs = [200
           350
           500];

  names = cellstr(num2str(freqs));

  saw_widths = [0.25 0.50 0.75 1.0];

  make_single_beeps(freqs, saw_widths, samp_freq, names);
  make_double_beeps(freqs, saw_widths, samp_freq, names);

  freqs = [329.6 % E
           349.2 % F
           370.0 % F#
           392.0 % G
           415.3 % G#
           440.0 % A
           466.2 % A#
           493.9 % B
           523.3 % C
           554.4 % C#
           587.3 % D
           622.3 % D#
           659.3 % E
          ];

  names = {'E_329'
           'F_349'
           'F#_370'
           'G_392'
           'G#_415'
           'A_440'
           'A#_466'
           'B_493'
           'C_523'
           'C#_554'
           'D_587'
           'D#_622'
           'E_659'
          };

  saw_widths = [0.50];

  make_single_beeps(freqs, saw_widths, samp_freq, names);

% 12 C  16.4  60  C  261    108  C  4186.0
% 13 C# 17.3   61 C# 277.2   109 C# 4434.9
% 14 D 18.4   62 D 293.7   110 D 4698.6
% 15 D# 19.4   63 D# 311.1   111 D# 4978.0
% 16 E 20.6   64 E 329.6   112 E 5274.0
% 17 F 21.8   65 F 349.2   113 F 5587.7
% 18 F# 23.1   66 F# 370.0   114 F# 5919.9
% 19 G 24.5   67 G 392.0   115 G 6271.9
% 20 G# 26.0   68 G# 415.3   116 G# 6644.9
% 21 A 27.5   69 A 440.0   117 A 7040.0
% 22 A# 29.1   70 A# 466.2   118 A# 7458.6
% 23 B 30.9   71 B 493.9   119 B 7902.1
%
% 24 C 32.7   72 C 523.3   120 C 8372.0
% 25 C# 34.6   73 C# 554.4   121 C# 8869.8
% 26 D 36.7   74 D 587.3   122 D 9397.3
% 27 D# 38.9   75 D# 622.3   123 D# 9956.1
% 28 E 41.2   76 E 659.3   134 E 10548.1
% 29 F 43.7   77 F 698.5   125 F 11175.3
% 30 F# 46.2   78 F# 740.0   126 F# 11839.8
% 31 G 49.0   79 G 784.0   127 G 12543.9
% 32 G# 51.9   80 G# 830.6   na G# 13289.8
% 33 A 55.0   81 A 880.0   na A 14080.0
% 34 A# 58.3   82 A# 932.3   na A# 14917.2
% 35 B 61.7   83 B 987.8   na B 15804.3
%
% 36 C 65.4   84 C 1046.5   na C 16744.0
% 37 C# 69.3   85 C# 1108.7   na C# 17739.7
% 38 D 73.4   86 D 1174.7   na D 18794.5
% 39 D# 77.8   87 D# 1244.5   na D# 19912.1
% 40 E 82.4   88 E 1318.5   na E 21096.2
% 41 F 87.3   89 F 1396.9   na F 22350.6
% 42 F# 92.5   90 F# 1480.0   na F# 23679.6
% 43 G 98.0   91 G 1568.0   na G 25087.7
% 44 G# 103.8   92 G# 1661.2   na G# 26579.5
% 45 A 110.0   93 A 1760.0   na A 28160.0
% 46 A# 116.5   94 A# 1864.7   na A# 29834.5
% 47 B 123.5   95 B 1975.5   na B 31608.5
%
% 48 C 130.8   96 C 2093.0   na C 33488.1
% 49 C# 138.6   97 C# 2217.5   na C# 35479.4
% 50 D 146.8   98 D 2349.3   na D 37589.1
% 51 D# 155.6   99 D# 2489.0   na D# 39824.3
% 52 E 164.8   100 E 2637.0   na E 42192.3
% 53 F 174.6   101 F 2793.8   na F 44701.2
% 54 F# 185.0   102 F# 2960.0   na F# 47359.3
% 55 G 196.0   103 G 3136.0   na G 50175.4
% 56 G# 207.7   104 G# 3322.4   na G# 53159.0
% 57 A 220.0   105 A 3520.0   na A 56320.0
% 58 A# 233.1   106 A# 3729.3   na A# 59669.0
% 59 B 246.9   107 B 3951.1   na B 63217.1


function make_single_beeps(freqs, saw_widths, samp_freq, names)

  dur = 0.3;

  tail = zeros(1, 0.1*samp_freq);

  taper_width = 441;
  taper = [linspace(0,1,taper_width)...
           ones(1,dur*samp_freq-2*taper_width)...
           linspace(1,0,taper_width)];

  for f = 1:length(freqs)
    freq = freqs(f);

    snd = taper .* sinwave(freq, dur, samp_freq);
    snd = [snd tail];
    fname = sprintf('sin_%sHz_%dms.au', names{f}, dur*1000);
    fprintf('generating %s\n', fname);
    auwrite(snd, samp_freq, fname);

    for w = 1:length(saw_widths)
      saw_width = saw_widths(w);

      snd = taper .* sawwave(freq, dur, samp_freq, saw_width);
      snd = [snd tail];
      fname = sprintf('saw%d_%sHz_%dms.au', ...
                      saw_width*100, names{f}, dur*1000);
      fprintf('generating %s\n', fname);
      auwrite(snd, samp_freq, fname);
    end
  end


function make_double_beeps(freqs, saw_widths, samp_freq, names)

  dur = 0.12;
  gapdur = 0.06;
  gap = zeros(1, floor(gapdur*samp_freq));
  tail = zeros(1, 0.1*samp_freq);

  taper_width = 441;
  taper = [linspace(0,1,taper_width)...
           ones(1,dur*samp_freq-2*taper_width)...
           linspace(1,0,taper_width)];

  for f = 1:length(freqs)
    freq = freqs(f);

    snd = taper .* sinwave(freq, dur, samp_freq);
    snd = [snd gap snd tail];
    fname = sprintf('sin_%sHz_2x%dms.au', names{f}, dur*1000);
    fprintf('generating %s\n', fname);
    auwrite(snd, samp_freq, fname);

    for w = 1:length(saw_widths)
      saw_width = saw_widths(w);

      snd = taper .* sawwave(freq, dur, samp_freq, saw_width);
      snd = [snd gap snd tail];
      fname = sprintf('saw%d_%sHz_2x%dms.au', ...
                      saw_width*100, names{f}, dur*1000);
      fprintf('generating %s\n', fname);
      auwrite(snd, samp_freq, fname);
    end
  end


function snd = sinwave(freq, secs, sample_freq)

  num_samples = secs * sample_freq;
  x = 0:(num_samples-1);
  x = x./sample_freq;
  snd = sin(2*pi*freq*x);

function snd = sawwave(freq, secs, sample_freq, width)

  num_samples = secs * sample_freq;
  x = 0:(num_samples-1);
  x = x./sample_freq;
  snd = sawtooth(2*pi*freq*x,width);
