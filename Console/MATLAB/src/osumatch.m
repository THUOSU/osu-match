%% osumatch: sync music
function [offset, audioMatch, fs1] = osumatch(audioFileHQ, audioFileRef, audioFileMatch)

% load audio files
try
    [audioHQ,fs1] = audioread(audioFileHQ);
    [audioRef,fs2] = audioread(audioFileRef);
catch err
    disp('Cannot Read File!');
    return;
end

% parameters
osuFs = 1000;
osuErr = ceil(fs1/1000);
halfErr = ceil(osuErr/2);
seg1Len = fs1;
seg2Len = 10*fs2;
threshold = 0.2;

try
    % find the first nonzero frame
    nonzero1 = max(find(audioHQ(:,1)>threshold,1,'first'),find(audioHQ(:,2)>threshold,1,'first'));
    nonzero2 = max(find(audioRef(:,1)>threshold,1,'first'),find(audioRef(:,2)>threshold,1,'first'));

    % pick the segment to match offset
    segment1 = mean(audioHQ(nonzero1:nonzero1+seg1Len-1,:).');
    segment2 = mean(audioRef(nonzero2:nonzero2+ceil(seg2Len*fs2/fs1)-1,:).');
catch err
    disp('File Not Supprot!');
    return;
end

% resample the segment to osu! beatmap sample rate
segR1 = resample(segment1,osuFs,fs1);
segR2 = resample(segment2,osuFs,fs2);


% BEGIN calc the offset

% calc the correlation function at osu! beatmap sample rate
R = conv(flip(segR1),segR2);

% pick the max correlaion as sync
[~,I] = max(R);
offset = (I-length(segR1))*fs1/osuFs+nonzero2-nonzero1;

% resample the segment of the reference audio to sample rate of high quality audio
if fs1 ~= fs2
    segment2 = resample(segment2,fs1,fs2);
end

% trim the segments
seg1 = segment1;
seg2 = segment2;
if offset > 0
    seg2 = segment2(1+offset:end);
elseif offset < 0
    seg1 = segment1(1-offset:end);
end

% calc the correlation function at high quality audio sample rate
% R = conv(flip(seg1),seg2);
seg1L = length(seg1);
R = zeros(1,2*halfErr+1);
R(1+halfErr) = sum(seg1(1+halfErr:seg1L).*seg2(1+halfErr:seg1L));
for k=1:halfErr
    R(1+halfErr-k) = sum(seg1(1+halfErr:seg1L).*seg2(1+halfErr-k:seg1L-k));
    R(1+halfErr+k) = sum(seg1(1+halfErr:seg1L).*seg2(1+halfErr+k:seg1L+k));
end

% pick the max correlaion as sync
[~,I] = max(R);
offset = offset+I-halfErr-1;

% END clac the offset

% apply offset to the high quality audio
if offset > 0
    audioMatch = [zeros(offset,size(audioHQ,2));audioHQ];
elseif offset < 0
    audioMatch = audioHQ(1-offset:end,:);
else
    audioMatch = audioHQ;
end

audiowrite(audioFileMatch,audioMatch,fs1);

