%% osumatch: sync music
function [offset, audioMatch, fs1] = osumatch(audioFileHQ, audioFileRef, audioFileMatch)

% parameters
seg1Len = 2^10;
seg2Len = 2^12;
threshold = 0.2;

% load audio files
[audioHQ,fs1] = audioread(audioFileHQ);
[audioRef,fs2] = audioread(audioFileRef);

% find the first nonzero frame
nonzero1 = max(find(audioHQ(:,1)>threshold,1,'first'),find(audioHQ(:,2)>threshold,1,'first'));
nonzero2 = max(find(audioRef(:,1)>threshold,1,'first'),find(audioRef(:,2)>threshold,1,'first'));

% pick the segment to match offset
segment1 = mean(audioHQ(nonzero1:nonzero1+seg1Len-1,:).');
segment2 = mean(audioRef(nonzero2:nonzero2+ceil(seg2Len*fs2/fs1)-1,:).');

% resample the segment of the reference audio to sample rate of high quality audio
segment2 = resample(segment2,fs1,fs2);

% BEGIN calc the offset

% calc the correlation function
R = conv(flip(segment1),segment2);

% pick the max correlaion as sync
[~,offset] = max(R);
offset = offset-seg1Len+nonzero2-nonzero1;

% apply offset to the high quality audio
if offset > 0
    audioMatch = [zeros(offset,size(audioHQ,2));audioHQ];
elseif offset < 0
    audioMatch = audioHQ(1-offset:end,:);
else
    audioMatch = audioHQ;
end

audiowrite(audioFileMatch,audioMatch,fs1);

