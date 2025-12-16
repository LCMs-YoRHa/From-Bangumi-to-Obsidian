<!-- src/components/tabs/BatchImportTab.vue -->
<!-- 批量导入选项卡 -->
<script setup lang="ts">
import { ref, computed } from 'vue';
import { useBangumi } from '@/composables/useBangumi';
import { useTaskStore } from '@/stores/task';
import { invoke } from '@tauri-apps/api/core';
import ProgressIndicator from '@/components/common/ProgressIndicator.vue';
import { IconFolder, IconLoading, IconSave, IconSettings } from '@/components/icons';
import {
  SubjectType,
  SubjectTypeLabels,
  CollectionTypeLabels,
  type BatchImportParams,
} from '@/types';

const { batchExportSubjects, error, isAuthenticated } = useBangumi();
const taskStore = useTaskStore();

// 表单状态
const subjectType = ref<number>(SubjectType.Anime);
const collectionType = ref<number | undefined>(undefined);
const limit = ref<number>(30);
const autoProcess = ref<boolean>(true);

// 文件导入状态
const selectedFile = ref<string | null>(null);
const fileContent = ref<string>('');
const parsedSubjectIds = ref<number[]>([]);
const isReadingFile = ref<boolean>(false);
const fileError = ref<string | null>(null);

// 选项数据
const subjectTypeOptions = Object.entries(SubjectTypeLabels).map(([value, label]) => ({
  value: Number(value),
  label: `${label} (${value})`,
}));

const collectionTypeOptions = [
  { value: undefined as number | undefined, label: '全部' },
  ...Object.entries(CollectionTypeLabels).map(([value, label]) => ({
    value: Number(value),
    label: `${label} (${value})`,
  })),
];

const canStart = computed(() => {
  return isAuthenticated.value && !taskStore.isRunning;
});

async function handleStart() {
  const params: BatchImportParams = {
    subject_type: subjectType.value,
    collection_type: collectionType.value,
    limit: limit.value,
    offset: 0,
    auto_process: autoProcess.value,
  };

  await batchExportSubjects(params);
}

// 文件选择和导入逻辑
async function selectFile() {
  try {
    isReadingFile.value = true;
    fileError.value = null;
    
    // 调用后端文件选择命令
    const filePath = await invoke<string>('select_file');
    if (filePath) {
      selectedFile.value = filePath;
      await readFileContent(filePath);
    }
  } catch (e) {
    fileError.value = `文件选择失败: ${e}`;
    console.error('文件选择失败:', e);
  } finally {
    isReadingFile.value = false;
  }
}

async function readFileContent(filePath: string) {
  try {
    isReadingFile.value = true;
    fileError.value = null;
    
    // 读取文件内容
    const content = await invoke<string>('read_file_content', { filePath });
    fileContent.value = content;
    
    // 解析文件中的条目ID
    parseSubjectIdsFromContent(content);
  } catch (e) {
    fileError.value = `读取文件失败: ${e}`;
    console.error('读取文件失败:', e);
  } finally {
    isReadingFile.value = false;
  }
}

function parseSubjectIdsFromContent(content: string) {
  // 尝试从文件内容中解析条目ID
  // 支持多种格式：每行一个ID、逗号分隔、空格分隔等
  const ids: number[] = [];
  
  // 按行分割并处理
  const lines = content.split(/[\n,;\s]+/);
  for (const line of lines) {
    const trimmed = line.trim();
    if (trimmed) {
      const id = parseInt(trimmed);
      if (!isNaN(id) && id > 0) {
        ids.push(id);
      }
    }
  }
  
  parsedSubjectIds.value = ids;
}

async function importFromFile() {
  if (parsedSubjectIds.value.length === 0) {
    fileError.value = '没有从文件中解析到有效的条目ID';
    return;
  }
  
  try {
    // 这里可以添加从文件导入的具体逻辑
    // 例如：调用batchExportSubjects并传入解析的ID列表
    console.log('从文件导入条目ID:', parsedSubjectIds.value);
    // 实际实现中，需要扩展useBangumi composable以支持从ID列表导入
  } catch (e) {
    fileError.value = `导入失败: ${e}`;
    console.error('导入失败:', e);
  }
}
</script>

<template>
  <div class="batch-import-tab">
    <div class="tab-header">
      <h3>批量导入</h3>
      <p class="tab-description">获取并导出您的全部收藏条目</p>
    </div>

    <div class="settings-section">
      <h4>导入设置</h4>

      <div class="form-row">
        <div class="form-group">
          <label class="form-label">条目类型</label>
          <select v-model="subjectType" class="form-select">
            <option
              v-for="option in subjectTypeOptions"
              :key="option.value"
              :value="option.value"
            >
              {{ option.label }}
            </option>
          </select>
        </div>

        <div class="form-group">
          <label class="form-label">收藏状态</label>
          <select v-model="collectionType" class="form-select">
            <option
              v-for="option in collectionTypeOptions"
              :key="option.value ?? 'all'"
              :value="option.value"
            >
              {{ option.label }}
            </option>
          </select>
        </div>
      </div>

      <div class="form-row">
        <div class="form-group">
          <label class="form-label">每次获取数量</label>
          <input
            v-model.number="limit"
            type="number"
            min="1"
            max="100"
            class="form-input"
          />
        </div>
      </div>

      <div class="form-group checkbox-group">
        <label class="checkbox-label">
          <input v-model="autoProcess" type="checkbox" />
          <span>获取完成后自动生成 Markdown 文件</span>
        </label>
      </div>
    </div>

    <!-- 文件导入区域 -->
    <div class="file-import-section">
      <h4>从文件导入</h4>
      <div class="file-import-content">
        <div class="file-selection">
          <button 
            class="btn btn-secondary" 
            @click="selectFile"
            :disabled="isReadingFile || taskStore.isRunning"
          >
            <IconSettings :size="16" class="btn-icon" />
            {{ isReadingFile ? '读取中...' : '选择文件' }}
          </button>
          
          <div v-if="selectedFile" class="selected-file">
            <span class="file-path">{{ selectedFile }}</span>
          </div>
        </div>
        
        <div v-if="fileError" class="error-message file-error">
          {{ fileError }}
        </div>
        
        <div v-if="fileContent" class="file-preview">
          <div class="preview-header">
            <h5>文件内容预览</h5>
            <span class="preview-info">{{ parsedSubjectIds.length }} 个有效条目ID</span>
          </div>
          <div class="preview-content">
            <pre>{{ fileContent }}</pre>
          </div>
          
          <button 
            class="btn btn-primary btn-sm" 
            @click="importFromFile"
            :disabled="parsedSubjectIds.length === 0 || taskStore.isRunning"
          >
            <IconSave :size="16" class="btn-icon" />
            从文件导入
          </button>
        </div>
      </div>
    </div>

    <div class="output-info">
      <IconFolder :size="18" />
      <span>生成的 Markdown 文件将保存到应用数据目录的 output 文件夹中</span>
    </div>

    <div class="error-message" v-if="error">
      {{ error }}
    </div>

    <div class="progress-section" v-if="taskStore.isRunning && taskStore.progress">
      <ProgressIndicator
        :current="taskStore.progress.current"
        :total="taskStore.progress.total"
        :message="taskStore.progress.message"
      />
    </div>

    <button
      class="btn btn-primary btn-large"
      :disabled="!canStart"
      @click="handleStart"
    >
      <template v-if="taskStore.isRunning">
        <IconLoading :size="18" />
        <span>导入中...</span>
      </template>
      <template v-else>
        <span>开始批量导入</span>
      </template>
    </button>
  </div>
</template>

<style scoped>
.batch-import-tab {
  padding: 20px;
}

.tab-header {
  margin-bottom: 24px;
}

.tab-header h3 {
  margin: 0 0 8px;
  font-size: 1.25rem;
  color: #333;
}

.tab-description {
  margin: 0;
  color: #666;
  font-size: 0.9rem;
}

.settings-section {
  background: #f9f9f9;
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}

.file-import-section {
  background: #f9f9f9;
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}

.file-import-section h4 {
  margin: 0 0 16px;
  font-size: 1rem;
  color: #555;
}

.file-import-content {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.file-selection {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.selected-file {
  padding: 8px 12px;
  background: white;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 0.875rem;
  color: #555;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.file-error {
  margin-top: 8px;
}

.file-preview {
  background: white;
  border: 1px solid #ddd;
  border-radius: 6px;
  padding: 12px;
}

.preview-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.preview-header h5 {
  margin: 0;
  font-size: 0.9rem;
  color: #555;
}

.preview-info {
  font-size: 0.8rem;
  color: #667eea;
  font-weight: 500;
}

.preview-content {
  max-height: 200px;
  overflow: auto;
  background: #f5f5f5;
  padding: 12px;
  border-radius: 4px;
  margin-bottom: 12px;
}

.preview-content pre {
  margin: 0;
  font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
  font-size: 0.8rem;
  line-height: 1.4;
  color: #333;
  white-space: pre-wrap;
  word-wrap: break-word;
}

.settings-section h4 {
  margin: 0 0 16px;
  font-size: 1rem;
  color: #555;
}

.form-row {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 16px;
  margin-bottom: 16px;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-label {
  margin-bottom: 6px;
  font-size: 0.875rem;
  font-weight: 500;
  color: #555;
}

.form-select,
.form-input {
  padding: 10px 12px;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 0.9rem;
  background: white;
}

.form-select:focus,
.form-input:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.checkbox-group {
  margin-top: 8px;
}

.checkbox-label {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
  font-size: 0.9rem;
  color: #555;
}

.checkbox-label input {
  width: 18px;
  height: 18px;
  cursor: pointer;
}

.output-info {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 12px 16px;
  background: rgba(102, 126, 234, 0.1);
  border-radius: 6px;
  color: #667eea;
  font-size: 0.875rem;
  margin-bottom: 16px;
}

.error-message {
  padding: 12px 16px;
  background: #ffebee;
  border-radius: 6px;
  color: #d32f2f;
  font-size: 0.875rem;
  margin-bottom: 16px;
}

.progress-section {
  margin-bottom: 16px;
}

.btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 12px 24px;
  border-radius: 6px;
  font-size: 1rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s;
  border: none;
}

.btn-sm {
  padding: 8px 16px;
  font-size: 0.875rem;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

.btn-primary:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
}

.btn-secondary {
  background: #f5f5f5;
  color: #333;
}

.btn-secondary:hover:not(:disabled) {
  background: #e0e0e0;
}

.btn-large {
  width: 100%;
  padding: 14px 24px;
}

.btn-icon {
  vertical-align: middle;
}
</style>
