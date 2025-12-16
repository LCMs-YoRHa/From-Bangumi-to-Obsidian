<!-- src/components/common/ProgressIndicator.vue -->
<!-- 进度指示器 -->
<script setup lang="ts">
import { computed } from 'vue';

const props = defineProps<{
  current: number;
  total: number;
  message?: string;
  showPercentage?: boolean;
}>();

const percentage = computed(() => {
  if (props.total === 0) return 0;
  return Math.round((props.current / props.total) * 100);
});

const progressStyle = computed(() => ({
  width: `${percentage.value}%`,
}));
</script>

<template>
  <div class="progress-indicator">
    <div class="progress-header">
      <span class="progress-message">{{ message || '处理中...' }}</span>
      <span class="progress-stats" v-if="showPercentage !== false">
        {{ current }} / {{ total }} ({{ percentage }}%)
      </span>
    </div>
    <div class="progress-bar-container">
      <div class="progress-bar" :style="progressStyle"></div>
    </div>
  </div>
</template>

<style scoped>
.progress-indicator {
  width: 100%;
}

.progress-header {
  display: flex;
  justify-content: space-between;
  margin-bottom: 8px;
  font-size: 0.875rem;
}

.progress-message {
  color: #333;
}

.progress-stats {
  color: #666;
}

.progress-bar-container {
  height: 8px;
  background: #e0e0e0;
  border-radius: 4px;
  overflow: hidden;
}

.progress-bar {
  height: 100%;
  background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
  border-radius: 4px;
  transition: width 0.3s ease;
}
</style>
