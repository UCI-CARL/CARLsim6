classdef PerformanceReader < handle
    % A PerformanceReader can be used to read performance counter files that were generated
    % with the PerformanceMonitor utility in CARLsim. The user can then directly
    % act on the returned performance counter data.
    %
    %
    % Example usage:
    % >> pR = PerformanceReader('results/p_CARLsim.dat');
    % >> pValues = pR.readValues();
    %
%
%
%    pR = PerformanceReader('results/p_CARLsim.dat');
%    pValues = pR.readValues();
%
%%
    
    %% PROPERTIES
    % public
    properties (SetAccess = private)
        fileStr;             % path to performance file
        errorMode;           % program mode for error handling
        supportedErrorModes; % supported error modes
    end
    
    % private
    properties (Hidden, Access = private)
        fileId;              % file ID of performance file
        fileSignature;       % int signature of all performance files
        fileVersionMajor;    % required major version number
        fileVersionMinor;    % required minimum minor version number
        fileSizeByteHeader;  % byte size of header section
        
        grid3D;              % 3D grid dimensions of group
        max_performance_mon_size;   % maximum number of neurons recorded in a group
        binWindow;           % binning window for recording values
        stimLengthMs;        % estimated simulation duration (determined by
                             % the time of last recording value)
        storeValues;         % flag whether to store/buffer neuron data
        nData;               % the buffered neuron data
        
        errorFlag;           % error flag (true if error occured)
        errorMsg;            % error message
    end
    
    
    %% PUBLIC METHODS
    methods
        function obj = PerformanceReader(performanceFile, storeValues, errorMode)
            % nR = NeuronReader(performanceFile) creates a new instance of class
            % NeuronReader, which can be used to read performance files generated
            % by the NeuronMonitor utility in CARLsim.
            %
            % performanceFile  - Path to performance file (expects data to be in
            %               recording value time ms) followed by core ID
            %               (both int32)) and values (all float), like
            %               the ones created by the CARLsim PerformanceMonitor
            %               utility.
            % storeValues - Flag whether to store/buffer the retrieved
            %               performance data. If enabled, repeated calls to
            %               nR.readValues will returned the buffered data
            %               instead of repeatedly re-reading the spike
            %               file. Default: true.
            % ERRORMODE   - Error Mode in which to run PerformanceReader. The
            %               following modes are supported:
            %                 - 'standard' Errors will be fatal (returned
            %                              via Matlab function error())
            %                 - 'warning'  Errors will be warnings
            %                              returned via Matlab function
            %                              warning())
            %                 - 'silent'   No exceptions will be thrown,
            %                              but object will populate the
            %                              properties errorFlag and
            %                              errorMsg.
            %               Default: 'standard'.
            obj.fileStr = performanceFile;
            obj.unsetError()
            obj.loadDefaultParams();
            
            if nargin<3
                obj.errorMode = 'standard';
            else
                if ~obj.isErrorModeSupported(errorMode)
                    obj.throwError(['errorMode "' errorMode '" is ' ...
                        ' currently not supported. Choose from the ' ...
                        'following: ' ...
                        strjoin(obj.supportedErrorModes, ', ') '.'], ...
                        'standard')
                    return
                end
                obj.errorMode = errorMode;
            end
            if nargin<2
                obj.storeValues = true;
            else
                obj.storeValues = storeValues;
            end
            if nargin<1
                obj.throwError('Path to spike file needed.');
                return
            end
            
            [~,~,fileExt] = fileparts(performanceFile);
            if strcmpi(fileExt,'')
                obj.throwError(['Parameter performanceFile must be a file ' ...
                    'name, directory found.'])
            end
            
            % move unsafe code out of constructor
            obj.openFile()
        end
        
        function delete(obj)
            % destructor, implicitly called to fclose file
            if obj.fileId ~= -1
                fclose(obj.fileId);
            end
        end
        
        function [errFlag,errMsg] = getError(obj)
            % [errFlag,errMsg] = getError() returns the current error
            % status.
            % If an error has occurred, errFlag will be true, and the
            % message can be found in errMsg.
            errFlag = obj.errorFlag;
            errMsg = obj.errorMsg;
        end
        
        function grid3D = getGrid3D(obj)
            % grid3D = nR.getGrid3D() returns the 3D grid dimensions of the
            % group as three-element vector <width x height x depth>. This
            % property is equal to the one set in CARLsim::createGroup.
            grid3D = obj.grid3D;
        end
        
        function max_performance_mon_size = getMaxPerformanceMonSize(obj)
            max_performance_mon_size = obj.max_performance_mon_size;
        end
        
        function simDurMs = getSimDurMs(obj)
            % simDurMs = nR.getSimDurMs() returns the estimated simulation
            % duration in milliseconds. This is equivalent to the time
            % stamp of the last rcording value that occurred. Other than that, a
            % NeuronReader does know about any simulation procedures.
            %
            % The total simulation duration is usually stored in a
            % "sim_{simName}.dat" file and can be retrieved by using a
            fseek(obj.fileId, -24, 'eof'); % jump to penultimate int
            simDurMs = fread(obj.fileId, 1, 'int32');
        end
        
        function nValues = readValues(obj)
            % nValues = nR.readValues(binWindow) reads the nueron file and
            % arranges spike times into bins of binWindow millisecond
            % length.
            %
            % Returns a struct with 4 fields: pdh_util
            % organized as (number of cores x duration of simulation (ms)).
           if obj.storeValues
                if  ~isempty(obj.nData)
                    % we don't need to re-read the data
                    nValue = obj.nData;
                    return;
                end
            end
            
            % extract stimulus length
            obj.stimLengthMs = obj.getSimDurMs();


            % rewind file pointer, skip header
            fseek(obj.fileId, obj.fileSizeByteHeader, 'bof');
            
            %data = fread(obj.fileId,[12 Inf],'uint8=>uint8');  % 1 counter 
            data = fread(obj.fileId,[24 Inf],'uint8=>uint8');   % 4 counter
            
            t = typecast(reshape(data(1:4,:),[],1),'uint32');
            core = typecast(reshape(data(5:8,:),[],1),'uint32');
            pdh_util = typecast(reshape(data(9:12,:),[],1),'single'); 
            pdh_ipc = typecast(reshape(data(13:16,:),[],1),'single'); 
            pdh_freq = typecast(reshape(data(17:20,:),[],1),'single'); 
            pdh_engy = typecast(reshape(data(21:24,:),[],1),'single'); 


			nCores = obj.grid3D(1);			
			dataSize = [nCores,  size(t,1)/nCores];

			% performance counter
            nValues.pdh_util = reshape(pdh_util, dataSize);
            nValues.pdh_ipc = reshape(pdh_ipc, dataSize);
            nValues.pdh_freq = reshape(pdh_freq, dataSize);
            nValues.pdh_engy = reshape(pdh_engy, dataSize); 

			% t and core as 1-dim vectors 
			nValues.t = reshape(t, dataSize)(1,:);   
			nValues.core = reshape(core, dataSize)(:,1);  

			nValues.nCores = nCores;
			nValues.ms = obj.stimLengthMs;
			
        
            % store neuron data
            if obj.storeValues
                obj.nData = nValues;
            end
            
        end
    end
    %% PRIVATE METHODS
    methods (Hidden, Access = private)
        function isSupported = isErrorModeSupported(obj, errMode)
            % determines whether an error mode is currently supported
            isSupported = sum(ismember(obj.supportedErrorModes,errMode))>0;
        end
        
        function loadDefaultParams(obj)
            % loads default parameter values for class properties
            obj.fileId = -1;
            obj.fileSignature = 206661982;
            obj.fileVersionMajor = 1;
            obj.fileVersionMinor = 0;
            obj.fileSizeByteHeader = -1; % to be set in openFile
            
            obj.grid3D = -1; % to be set in openFile
            obj.max_performance_mon_size = -1;
            
            obj.nData = []; % to be set in readValues
            obj.stimLengthMs = -1;
            
            obj.supportedErrorModes = {'standard', 'warning', 'silent'};

            % disable backtracing for warnings and errors
            warning off backtrace
        end
        
        function openFile(obj)
            % SR.openFile() reads the header section of the spike file and
            % sets class properties appropriately.
            obj.unsetError()
            
            % try to open spike file, try little-endian
            obj.fileId = fopen(obj.fileStr, 'r', 'l');
            if obj.fileId==-1
                obj.throwError(['Could not open file "' obj.fileStr ...
                    '" with read permission'])
                return
            end
            
            % read signature
            sign = fread(obj.fileId, 1, 'int32');
            if feof(obj.fileId)
                obj.throwError('File is empty.');
            else
                if sign~=obj.fileSignature
                    % try big-endian instead
                    fclose(obj.fileId);
                    obj.fileId = fopen(obj.fileStr, 'r', 'b');
                    sign = fread(obj.fileId, 1, 'int32');
                    if sign~=obj.fileSignature
                        obj.throwError(['Unknown file type: ' num2str(sign)]);
                        return
                    end
                end
            end          

            % read version number
            version = fread(obj.fileId, 1, 'float32');
            if feof(obj.fileId) || floor(version) ~= obj.fileVersionMajor
                % check major number: must match
                obj.throwError(['File must be of version ' ...
                    num2str(obj.fileVersionMajor) '.x (Version ' ...
                    num2str(version) ' found'])
                return
            end
            if feof(obj.fileId) ...
                    || floor((version-obj.fileVersionMajor)*10.01)<obj.fileVersionMinor
                % check minor number: extract first digit after decimal
                % point
                % multiply 10.01 instead of 10 to avoid float rounding
                % errors
                obj.throwError(['File version must be >= ' ...
                    num2str(obj.fileVersionMajor) '.' ...
                    num2str(obj.fileVersionMinor) ' (Version ' ...
                    num2str(version) ' found)'])
                return
            end
            
            % read Grid3D
            obj.grid3D = fread(obj.fileId, [1 4], 'int32');
            if feof(obj.fileId) || prod(obj.grid3D)<=0
                obj.throwError(['Could not find valid Grid3D ' ...
                    'dimensions (grid=[' num2str(obj.grid3D(1)) ' ' ...
                    num2str(obj.grid3D(2)) ' ' num2str(obj.grid3D(4)) ...
                    '])'])
                return
            end
            
            %% read maximum number of neurons in a grouprecorded with NeuronMonitor
            %obj.max_performance_mon_size = fread(obj.fileId, 1, 'int32');
            
            % store the size of the header section, so that we can skip it
            % when re-reading spikes
            obj.fileSizeByteHeader = ftell(obj.fileId);
        end
        
        function throwError(obj, errorMsg, errorMode)
            % SR.throwError(errorMsg, errorMode) throws an error with a
            % specific severity (errorMode). In all cases, obj.errorFlag is
            % set to true and the error message is stored in obj.errorMsg.
            % Depending on errorMode, an error is either thrown as fatal,
            % thrown as a warning, or not thrown at all.
            % If errorMode is not given, obj.errorMode is used.
            if nargin<3,errorMode=obj.errorMode;end
            obj.errorFlag = true;
            obj.errorMsg = errorMsg;
            if strcmpi(errorMode,'standard')
                error(errorMsg)
            elseif strcmpi(errorMode,'warning')
                warning(errorMsg)
            end
            fclose(obj.fileId);
        end
        
        function unsetError(obj)
            % unsets error message and flag
            obj.errorFlag = false;
            obj.errorMsg = '';
        end
    end
end