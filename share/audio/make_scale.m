function s = make_scale(fundamental_freq, tempering)

% f1  A1
% f2  A2
% f3  E2
% f4  A3
% f5  C#3
% f6  E3
% f7  ??
% f8  A4
% f9  B4

% up M2  9:8
% up m3  6:5
% up M3  5:4
% up P4  4:3
% up P5  3:2
% up M6  5:3
% up P8  2:1

%    eq-temper interval   A-overtone
% A   440.0       P1       440.0
% A#  466.2       m2
% B   493.9       M2       495.0    (9:8)
% C   523.3       m3       528.0    (6:5)
% C#  554.4       M3       550.0    (5:4)
% D   587.3       P4       586.667  (4:3)
% D#  622.3       A4       622.254  (sqrt(2):1)
% E   659.3       P5       660.0    (3:2)
% F   698.5       m6       704.0    (8:5)
% F#  740.0       M6       733.333  (5:3)
% G   784.0       m7       782.222  (16:9)
% G#  830.6       M7       825.0    (15:8)
% A   880.0       P8       880.0    (2:1)

  pythag_ratios = [
      1
      9/8
      5/4
      4/3
      3/2
      5/3
      15/8
      2
      ];

  eqtemper_step = log(2)/12;

  eqtemper_ratios = [
      1
      exp( 2*eqtemper_step )
      exp( 4*eqtemper_step )
      exp( 5*eqtemper_step )
      exp( 7*eqtemper_step )
      exp( 9*eqtemper_step )
      exp( 11*eqtemper_step )
      2
      ];

  switch tempering
   case 'pythag'
    notes = make_tempered_scale(fundamental_freq, pythag_ratios);
   case 'eqtemper'
    notes = make_tempered_scale(fundamental_freq, eqtemper_ratios);
  end

  s = notes(:);

  I_chord = combine_notes(notes(:,[1 3 5]));
  IV_chord = combine_notes(notes(:,[4 6 8]));
  V_chord = combine_notes(notes(:,[2 5 7]));
  I_chord2 = combine_notes(notes(:,[3 5 8]));

  silence = zeros(4410, 1);

  s = [s; silence; ...
       I_chord; silence; IV_chord; silence; V_chord; silence; I_chord2; silence];

  s = 0.95 * s;

function ch = combine_notes(notes)

  ch = sum(notes,2);
  ch = ch ./ max(abs(ch));

function notes = make_tempered_scale(f, ratios)

  dur = 0.5;
  sf = 44100;

  nsamp = dur*sf;
  ntaper = ceil(0.02*sf);

  taper_in = linspace(0, 1, ntaper);
  taper_out = linspace(1, 0, ntaper);

  taper = [taper_in(:); ones(nsamp-2*ntaper,1); taper_out(:)];

  notes = zeros(nsamp, length(ratios));

  for n = 1:length(ratios)
    p1 = sawwave(f*ratios(n), dur, sf, 0.5, 0);
    p2 = sinwave(f*ratios(n), dur, sf);
    notes(:,n) = combine_notes([p1 3*p2]) .* taper;
  end
