classdef GroupReader < handle
    % A GroupReader can be used to read the group state file that was
    % generated with the GroupMonitor utility in CARLsim. The user can
    % directly act on the returned group state data, to access neurotransmitter 
    % concentration at specific times.
    %
    % To conveniently plot connection properties, please refer to
    % ConnectionMonitor.
    %
    % Example usage:
    % >>    GR = GroupReader('./results/grp_post-ex.dat');
    % >>    [timestamps, data] = GR.readData();
    % >>    GR.getMode();
    % >>    GR.getNRecords();
    % >> hist(allWeights(end,:))
    % >> % etc.
    %
    % Version 10/3/2020
    % Author: Lars Niedermeier <lars.niedermeier@swissonline.ch>
    
    %% PROPERTIES
    % public
    properties (SetAccess = private)
        fileStr;             % path to connect file
        errorMode;           % program mode for error handling
        supportedErrorModes; % supported error modes
    end
    
    % private
    properties (Hidden, Access = private)
        fileId;                % file ID of spike file
        fileSignature;         % int signature of all spike files
        fileVersionMajor;      % required major version number
        fileVersionMinor;      % required minimum minor version number
        fileSizeByteHeader;    % byte size of header section
        %fileSizeByteSnapshot; % byte size of a single snapshot
        fileSizeByteRecord;    % byte size of a single record data
        
        timeStamps;
        data;                  % DA or (DA,5HT,ACh,NE)
       
        nRecords;              % data length
        
        %nSnapshots;           % number of matrix snapshots 
       
        grpId;
		    grid;

        mode;                  % 0=DA, 1=ALL   %V2: bitmap
        
        errorFlag;           % error flag (true if error occured)
        errorMsg;            % error message
    end
    
    %% PUBLIC METHODS
    methods
        function obj = GroupReader(groupFile, errorMode)
            obj.fileStr = groupFile;
            obj.unsetError();
            obj.loadDefaultParams();
            
            if nargin<2
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
            if nargin<1
                obj.throwError('Path to connect file needed.');
                return
            end
            
            [~,~,fileExt] = fileparts(groupFile);
            if strcmpi(fileExt,'')
                obj.throwError(['Parameter connectFile must be a file ' ...
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
            % [errFlag,errMsg] = CR.getError() returns the current error
            % status.
            % If an error has occurred, errFlag will be true, and the
            % message can be found in errMsg.
            errFlag = obj.errorFlag;
            errMsg = obj.errorMsg;
        end
		
    		function grid3D = getGrid3D(obj)
    			% grid3D = GR.getGrid3D() returns the 3D grid dimensions for
    			% the group (1x3 vector)
    			grid3D = obj.grid;
    		end
 
    		function mode = getMode(obj)
    			mode = obj.mode;
    		end 
        
    		function nRecords = getNRecords(obj)
    			nRecords = obj.nRecords;
    		end 
        
        
        % step 1 DA only = data 
        function [timeStamps, data] = readData(obj)  % range
            %if nargin<2 || isempty(snapShots) || snapShots==-1
            %    snapShots = 1:obj.nSnapshots;
            %end
            
            %if snapShots==0
            %    obj.throwError('snapShots must be a list of snapshots.')
            %    return
            %end
            
            % dim performance opti
            obj.timeStamps = zeros(obj.nRecords, 1);
            obj.data = zeros(obj.nRecords, 1+obj.mode*3);
            
            %for i=1:numel(snapShots)
            %    frame = snapShots(i);

                % rewind file pointer, skip header
                fseek(obj.fileId, obj.fileSizeByteHeader, 'bof');
                
                %if frame>1
                %    % skip (frame-1) snapshots
                %    szByteToSkip = obj.fileSizeByteSnapshot*(frame-1);
                %    status = fseek(obj.fileId, szByteToSkip, 'cof');
                %    if status==-1
                %        obj.throwError(ferror(obj.fileId))
                %        return
                %    end
                %end
                
                % read data and append  to member
                %obj.timeStamps = [obj.timeStamps fread(obj.fileId, 1, 'int32')];
                %obj.data(end+1,:) = fread(obj.fileId, 1, 'float32');   % 1 or 4
            %end
            
            for i=1:obj.nRecords
                obj.timeStamps(i,1) = fread(obj.fileId, 1, 'int32');  % (end+1,1) would work as well 
                obj.data(i,:) = fread(obj.fileId, 1+obj.mode*3, 'float32');   % Read 1 or 4 elements, returning a column vector 
            end
              
            timeStamps = obj.timeStamps;
            data = obj.data;
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
            obj.fileSignature = 206661989; 
            obj.fileVersionMajor = 0;
            obj.fileVersionMinor = 2;
            obj.fileSizeByteHeader = -1;   % to be set in openFile
            %obj.fileSizeByteSnapshot = -1; % to be set in openFile
            obj.fileSizeByteRecord = 4*(2+obj.mode*3);
            
            obj.timeStamps = [];  % to be set in readWeights
            obj.data = [];     % to be set in readWeights
            %obj.data4 = [];     % to be set in readWeights

            obj.grpId = -1;

            %obj.nSnapshots = -1;
            
            obj.supportedErrorModes = {'standard', 'warning', 'silent'};

			% disable backtracing for warnings and errors
			warning off backtrace
        end
        
        function openFile(obj)
            % SR.openFile() reads the header section of the spike file and
            % sets class properties appropriately.
            obj.unsetError()
            
            % try to open connect file, use little-endian
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
            
            %% read group ID
            %obj.grpId = fread(obj.fileId, 1, 'int32');
            %if feof(obj.fileId) || obj.grpId<0
            %    obj.throwError(['Could not find valid group ID.'])
            %    return
            %end
            
            % skip grid3D
            fread(obj.fileId, 3, 'int32');
            if feof(obj.fileId) 
                obj.throwError(['Could not find valid group ID.'])
                return
            end
            
            %% read number of timestamps
            %obj.nTimestamps = fread(obj.fileId, 1, 'int32');
            %if feof(obj.fileId) || obj.nTimestamps<0
            %    obj.throwError(['Could not find valid number of ' ...
					  %'timestamps (' num2str(obj.nTimestamps) ')'])
            %    return
            %end
            

            % read mode 
            obj.mode = fread(obj.fileId, 1, 'int32');
            if feof(obj.fileId) || obj.mode<0 || obj.mode>1
                obj.throwError(['Could not find valid number of ' ...
					'mode (' num2str(obj.mode) ')'])
                return
            end
            
            % store the size of the header section, so that we can skip it
            % when re-reading spikes
            obj.fileSizeByteHeader = ftell(obj.fileId);
            
           
            % timestamp+1|4*data = sizeof(int32) + sizeof(float32)*(1|4) 
            obj.fileSizeByteRecord = 4+4*(1+obj.mode*3); 
            
            
            % compute number of records present in the file
            % find byte size from here on until end of file, divide it by
            % byte size of each snapshot -> number of snapshots
            fseek(obj.fileId, 0, 'eof');
            szByteTot = ftell(obj.fileId);
            %obj.nSnapshots = floor( (szByteTot-obj.fileSizeByteHeader) ...
            %    / obj.fileSizeByteSnapshot );
            obj.nRecords = floor( (szByteTot - obj.fileSizeByteHeader) ...
                / obj.fileSizeByteRecord );
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
        end
        
        function unsetError(obj)
            % unsets error message and flag
            obj.errorFlag = false;
            obj.errorMsg = '';
        end
    end
end